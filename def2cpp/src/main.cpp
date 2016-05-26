/*
Copyright 2015, Krauss Maffei Wegmann GmbH & Co. KG.
All rights reserved.
Redistribution and use in source and binary forms 
requires written permission of the copyright holder.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


@author Tomas Prerovsky <tomas.prerovsky@gmail.com>
*/


#include "main.hpp"
#include <set>

#define VERSION_DEF2CPP_MAJOR "0"
#define VERSION_DEF2CPP_MINOR "6"




std::string proc_ceps_func = R"( 

void process_ceps_file(	std::string const & file_name,
						ceps::Ceps_Environment& ceps_env,
						ceps::ast::Nodeset& universe )
{
 std::fstream ceps_file{file_name};
 if (!ceps_file) throw std::runtime_error("Couldn't open file:"+file_name);
 Ceps_parser_driver driver{ceps_env.get_global_symboltable(),ceps_file};
 ceps::Cepsparser parser{driver};
 if (parser.parse() != 0 || driver.errors_occured())
  throw std::runtime_error("Syntax error.");
 std::vector<ceps::ast::Nodebase_ptr> generated_nodes;
 ceps::interpreter::evaluate(universe,
							driver.parsetree().get_root(),
							ceps_env.get_global_symboltable(),
							ceps_env.interpreter_env(),
							&generated_nodes
							);
}//process_ceps_file



)";

ceps::ast::Nodeset*	current_universe;

ceps::Ceps_Environment * ceps_env_current;

const auto ERR_FILE_OPEN_FAILED = 0;
const auto ERR_CEPS_PARSER      = 1;

std::set<std::string> kmw4log_event_names;
std::map<std::string, std::pair<int, std::string> > kmw4log_events;
int kmw4log_events_counter = 0;
//Central data structures
std::set<std::string> kmw4log_state_names;
using kmw4log_states_t = std::map<std::string, std::tuple<int, std::string, std::vector<ceps::ast::Nodebase_ptr >,ceps::ast::Nodebase_ptr  > >;
kmw4log_states_t kmw4log_states;
int kmw4log_state_counter = 0;
std::map<std::string,std::vector<std::string>> log4kmw_record_contents;

static int indentation = 0;

void print_indentation(ostream& out, int n)
{
	for(int i = 0; i < n; ++i)
		out << " ";
}

int indent_incr(){
	return ++indentation;
}

int indent_decr(){
	return --indentation;
}

std::ostream& indent(std::ostream& os) { print_indentation(os,indentation); return os;}

struct gen_cpp_block{
	int t;
	bool bracketed = false;
	std::string lbr="{";
	std::string rbr = "}";
	std::ostream* os = nullptr;

	gen_cpp_block():t(indentation){indent_incr();}
	gen_cpp_block(std::ostream* o,bool b = false,std::string lbr_="{",std::string rbr_="}"):t(indentation),bracketed(b),lbr(lbr_),rbr(rbr_),os(o){
		if (bracketed && os) { indent(*os) << lbr << "\n";}
		indent_incr();
	}
	~gen_cpp_block(){indentation=t;if (bracketed && os) {  indent(*os) << rbr<<"\n";}}
};


void fatal(int code, std::string const & msg )
{
	stringstream ss;
	if (ERR_FILE_OPEN_FAILED == code)
		ss << "Couldn't open file '" << msg <<"'.";
	else if (ERR_CEPS_PARSER == code)
		ss << "A parser exception occured in file '" << msg << "'.";
	throw runtime_error{ss.str()};
}

void warn(int code, std::string const & msg)
{
	if (WARN_XML_PROPERTIES_MISSING_PREFIX_DEFINITION == code)
		cerr << "\n***WARNING. No xml file prefix defined for '"
		<< msg
		<< "', will use default (empty string).\nIf you want different behaviour please add the following to the global namespace:\n\txml{gen_file_prefix{"
		<< msg
		<< "{\"MY_XML_FILENAME_PREFIX_HERE\";};};.\n";
	else if (WARN_CANNOT_FIND_TEMPLATE == code)
		cerr << "\n***WARNING. No template found which matches the path "
		<< msg
		<< "." << endl;
	else if (WARN_NO_INVOCATION_MAPPING_AND_NO_TABLE_DEF_FOUND == code)
		cerr << "\n***WARNING. There exists neither a xml invocation mapping nor a db2 table definition for  "
		<< msg
		<< ". No file for this particular object will be generated." << endl;
	else if (WARN_TESTCASE_ID_ALREADY_DEFINED == code)
		cerr << "\n***WARNING. Id already defined in another testcase precondition: "
		<< msg
		<< "." << endl;
	else if (WARN_TESTCASE_EMPTY == code)
		cerr << "\n***WARNING. Testcase precondition is empty: "
		<< msg
		<< "." << endl;
}

/*
Iterates through a vector of file names, evaluates each file with ceps, appends resulting node set to the 'universe'.
Node set resulting from evaluation is supplemented with internal variables (like paths).
*/
void process_def_files(	vector<string> const & file_names,
						ceps::Ceps_Environment& ceps_env,
						string& last_file_processed,
						ceps::ast::Nodeset& universe )
{
	for(auto const & file_name : file_names)
	{
		if (DEBUG_MODE) cout << "***Processing file: " << file_name << endl;
		std::fstream def_file{ last_file_processed = file_name};
		if (!def_file) fatal(ERR_FILE_OPEN_FAILED,file_name);
		Ceps_parser_driver driver{ceps_env.get_global_symboltable(),def_file};
		ceps::Cepsparser parser{driver};

		if (parser.parse() != 0 || driver.errors_occured())
			fatal(ERR_CEPS_PARSER, file_name);

		std::vector<ceps::ast::Nodebase_ptr> generated_nodes;

		std::string lib_name;


		ceps::interpreter::evaluate(universe,
									driver.parsetree().get_root(),
									ceps_env.get_global_symboltable(),
									ceps_env.interpreter_env(),
									&generated_nodes
									);


		for (auto & node : generated_nodes)
		{
			if (!is_struct(node)) continue;
			Struct & strct =  as_struct_ref(node);

			if (!is_test_precondition(strct)) continue;
			auto file_info = new Struct("__fileinfo_output_dir",nullptr,nullptr,nullptr);
			strct.children().push_back( file_info );
			file_info->children().push_back(new String(get_rel_out_path(file_name), nullptr, nullptr, nullptr));
		}//for

		if (DEBUG_MODE) cout << "***Content: "  << endl;
		if (DEBUG_MODE) cout << ceps::ast::Nodebase::pretty_print << ceps::ast::Nodeset{generated_nodes} << std::endl;

	}//for
}//process_def_files




string escape_string(string const & s)
{
	string result;
	for(size_t i = 0; i < s.length();++i)
	{
		if (s[i] == '"')
			result += "\\\"";
		else if (s[i] == '\\')
			result += "\\\\";
		else
			result += s[i];
	}
	return result;
}



std::string gen_cpp_default_header_for_logger_client(){
	static auto s = R"(
#include <iostream>
#include <type_traits>
#include <tuple>
#include <cstdint>
#include <memory>
#include <iterator>
#include <limits>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include "log4kmw_states.hpp"
#include "log4kmw_records.hpp"
#include "log4kmw_loggers.hpp"

#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#else
#endif
)";

	return s;
}

std::string gen_cpp_default_header_for_meta_information(){
	static auto s = R"(#include "ceps_all.hh")";
	return s;
}






bool is_id_or_symbol(ceps::ast::Nodebase_ptr node, std::string& kind, std::string& name){
	if (node->kind() == ceps::ast::Ast_node_kind::identifier) {
		name = ceps::ast::name(ceps::ast::as_id_ref(node)); return true;
	}


	if (node->kind() == ceps::ast::Ast_node_kind::symbol){
		auto& sym = ceps::ast::as_symbol_ref(node);
		name = ceps::ast::name(sym);

		if (kind.length() == 0) {kind = ceps::ast::kind(sym); return true;}
		return kind == ceps::ast::kind(sym);
	}
	return false;
}

bool ns_is_id_or_symbol(ceps::ast::Nodeset const& ns, std::string kind, std::string& name){
	if (ns.nodes().size()!=1) return false;
	return is_id_or_symbol(ns.nodes()[0], kind,name);
}


bool is_string(ceps::ast::Nodebase_ptr node, std::string& value){
	if (node->kind() != ceps::ast::Ast_node_kind::string_literal)return false;

	value = ceps::ast::value(ceps::ast::as_string_ref(node)); return true;
}

bool ns_is_string(ceps::ast::Nodeset const& ns, std::string& value){
	if (ns.nodes().size()!=1) return false;
	return is_string(ns.nodes()[0],value);
}

bool is_int(ceps::ast::Nodebase_ptr node, int& value){
	if (node->kind() != ceps::ast::Ast_node_kind::int_literal)return false;
		value = ceps::ast::value(ceps::ast::as_int_ref(node)); return true;
}

bool ns_is_int(ceps::ast::Nodeset const& ns, int& value){
	if (ns.nodes().size()!=1) return false;
	return is_int(ns.nodes()[0],value);
}


bool is_state_assignment(Nodebase_ptr node,std::string& state,Nodebase_ptr & rhs){
	if (node->kind() != ceps::ast::Ast_node_kind::binary_operator || ceps::ast::op(ceps::ast::as_binop_ref(node)) != '=')
		return false;
	auto& root = ceps::ast::as_binop_ref(node);
	std::string systemstate_kind = "Systemstate";

	if(!is_id_or_symbol(root.left(), systemstate_kind, state)) return false;

	rhs=root.right();

	return true;

}



void flatten_args(ceps::ast::Nodebase_ptr r, std::vector<ceps::ast::Nodebase_ptr>& v, char op_val = ',')
{
	if (r == nullptr) return;
	if (r->kind() == ceps::ast::Ast_node_kind::binary_operator && op(as_binop_ref(r)) ==  op_val)
	{
		auto& t = as_binop_ref(r);
		flatten_args(t.left(),v,op_val);
		flatten_args(t.right(),v,op_val);
		return;
	}
	v.push_back(r);
}

bool read_func_call_values( ceps::ast::Nodebase_ptr root_node,
							std::string & func_name,
							std::vector<ceps::ast::Nodebase_ptr>& args)
{
	if (root_node->kind() != ceps::ast::Ast_node_kind::func_call) return false;
	try
	{
		using namespace ceps::ast;
		ceps::ast::Func_call& func_call = *dynamic_cast<ceps::ast::Func_call*>(root_node);
		ceps::ast::Identifier& id = *dynamic_cast<ceps::ast::Identifier*>(func_call.children()[0]);
		func_name = name(id);
		args.clear();
		if (nlf_ptr(func_call.children()[1])->children().size()) flatten_args(nlf_ptr(func_call.children()[1])->children()[0],args);
	} catch (...)
	{
		return false;
	}
	return true;
}



std::string gen_cpp_test_run_signature(std::string test_name){
	return "void test_"+test_name+"()";
}

std::string gen_cpp_init_logger(std::string logger,std::string persistency ){
	return "log4kmw_loggers::logger_"+logger+".logger().init("+persistency+");";
}

std::string gen_cpp_mmapped_file(std::string path,size_t size){
	return "log4kmw::persistence::memory_mapped_file(\""+path+"\", "+std::to_string(size)+", true)";
}


std::string gen_cpp_unit(ceps::ast::Unit_rep u){
	return std::to_string(u.m)+","+std::to_string(u.kg)+","+std::to_string(u.s)+","+
		   std::to_string(u.ampere)+","+std::to_string(u.kelvin)+","+std::to_string(u.mol)+","+std::to_string(u.candela);
}

std::string gen_cpp_state_expression(Nodebase_ptr rhs){

	if (rhs->kind() == ceps::ast::Ast_node_kind::float_literal){
		auto u = gen_cpp_unit(ceps::ast::unit(ceps::ast::as_double_ref(rhs)));
		return "log4kmw::State<SI::Quantity<SI::Unit<"+u+"> ,double> >("+std::to_string(ceps::ast::value(ceps::ast::as_double_ref(rhs)))+")";
	} else 	if (rhs->kind() == ceps::ast::Ast_node_kind::int_literal){
		auto u = gen_cpp_unit(ceps::ast::unit(ceps::ast::as_int_ref(rhs)));
	}
	else if (rhs->kind() == ceps::ast::Ast_node_kind::string_literal){
		return "\""+ceps::ast::value(ceps::ast::as_string_ref(rhs))+"\"";
	}

	throw std::runtime_error("gen_cpp_state_expression:Unsupported expression\n");
}

std::string gen_cpp_state_assignment(std::string state,Nodebase_ptr rhs){
	return "log4kmw_states::write_state_"+state+"(" + gen_cpp_state_expression(rhs) +")";
}

std::string gen_cpp_log_event(std::string event,std::string logger){
	return "log4kmw_loggers::log_event(log4kmw_events::"+event+"(), log4kmw_loggers::"+logger+")";
}

std::string gen_cpp_func_call(std::string func_name,std::vector<ceps::ast::Nodebase_ptr>& args){
	std::string r = func_name+"(";
	for(int i = 0; i != args.size();++i){
		if (args[i]->kind() == ceps::ast::Ast_node_kind::identifier)
			r += ceps::ast::name(ceps::ast::as_id_ref(args[i]));
		if (i+1 < args.size()) r+=",";
	}
	r+=")";
	return r;
}

std::string gen_cpp_method_call(std::string lhs_id_or_sym,std::string kind, std::string func_name,std::vector<ceps::ast::Nodebase_ptr>& args){
	std::string r;
	if (kind == "Systemstate") r = "log4kmw::get_value<0>("+lhs_id_or_sym+")."+func_name+"("; else r = lhs_id_or_sym+"."+func_name+"(";
	for(int i = 0; i != args.size();++i){
		if (args[i]->kind() == ceps::ast::Ast_node_kind::identifier)
			r += ceps::ast::name(ceps::ast::as_id_ref(args[i]));
		else if (args[i]->kind() == ceps::ast::Ast_node_kind::int_literal){
			auto&  v = ceps::ast::as_int_ref(args[i]);
			if (unit(v) == ceps::ast::all_zero_unit()){
				r+= std::to_string(value(v));
			}
		}
		if (i+1 < args.size()) r+=",";
	}
	r+=")";
	return r;
}




bool is_method_call(ceps::ast::Nodebase_ptr node,std::string& lhs_id_or_sym,std::string& kind, std::string& func_name,std::vector<ceps::ast::Nodebase_ptr>& args){
	if (node->kind() != ceps::ast::Ast_node_kind::binary_operator) return false;
	auto& bin_op = ceps::ast::as_binop_ref(node);
	if (ceps::ast::op(bin_op) != '.') return false;
	if (!is_id_or_symbol(bin_op.left(),kind,lhs_id_or_sym)) return false;
	if(!read_func_call_values( bin_op.right(),
							   func_name,
							   args)) return false;
	return true;
}

bool is_func_call(ceps::ast::Nodebase_ptr node, std::string& func_name,std::vector<ceps::ast::Nodebase_ptr>& args){
	if (node->kind() != ceps::ast::Ast_node_kind::func_call) return false;
	if(!read_func_call_values( node,
							   func_name,
							   args)) return false;
	return true;
}


std::string gen_cpp_logger_type(std::string id){
	return "log4kmw::Logger<log4kmw_records::"+id+ "_t, log4kmw::persistence::memory_mapped_file>";
}

std::string gen_cpp_record_type(std::string id){
	return id+ "_t";
}

std::string gen_cpp_loggers_nmspc() {return "log4kmw_loggers::";}

std::string const & gen_cpp_utils() {return proc_ceps_func;}

void write_copyright_and_timestamp(std::ostream& out, std::string title,bool b){
	if(!b) return;
	time_t timer;time(&timer);tm * timeinfo;timeinfo = localtime(&timer);
	out
		<< "/* "<< title <<" " << std::endl
		<< "   CREATED " << asctime(timeinfo) << std::endl
		<< "   GENERATED BY logspec2cpp VERSION " << VERSION_DEF2CPP_MAJOR << "." << VERSION_DEF2CPP_MINOR << " (c) Krauss-Maffei-Wegmann GmbH & Co. KG. \n"
		<< "   BASED ON CEPS "<< ceps::get_version_info() << std::endl
		<< "   THIS IS A GENERATED FILE. DO NOT MODIFY.\n*/\n"
		<< std::endl << std::endl;
}

void write_kmw4log_events_artifacts(
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line
	)
{
	if (kmw4log_events.size() == 0) return;
	
	std::string out_path = cmd_line.out_path;

	if (DEBUG_MODE) std::cout << "Outpath: " << out_path << std::endl;
	
	string kmw4log_events_hpp_out_name{ out_path + "log4kmw_events.hpp" };
	string kmw4log_events_cpp_out_name{ out_path + "log4kmw_events.cpp" };

	mk_directories(out_path);



	std::ofstream kmw4log_events_hpp_file_out{ kmw4log_events_hpp_out_name };
	std::ofstream kmw4log_events_cpp_file_out{ kmw4log_events_cpp_out_name };
	
	if (!kmw4log_events_hpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_events_hpp_out_name + "'\n");
	if (!kmw4log_events_cpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_events_cpp_out_name + "'\n");

	write_copyright_and_timestamp(kmw4log_events_hpp_file_out,"LOG4KMW_EVENTS.HPP",print_header_in_log4kmw_events_files);
	write_copyright_and_timestamp(kmw4log_events_hpp_file_out,"LOG4KMW_EVENTS.CPP",print_header_in_log4kmw_events_files);

	kmw4log_events_hpp_file_out << "#ifndef LOG4KMW_EVENTS_HPP_INC\n";
	kmw4log_events_hpp_file_out << "#define LOG4KMW_EVENTS_HPP_INC\n\n";
	kmw4log_events_hpp_file_out << "#include \"log4kmw_event.hpp\"\n\n\n";
	kmw4log_events_hpp_file_out << "namespace log4kmw_events{\n";

	for (auto & ev : kmw4log_events)
	{
		kmw4log_events_hpp_file_out << " typedef log4kmw::Event<" << ev.second.first << "> " << ev.first << ";\n";
	}//for

	


	kmw4log_events_hpp_file_out << "}\n\n";
	kmw4log_events_hpp_file_out << "#endif\n";

	kmw4log_events_cpp_file_out << "#include \"log4kmw_events.hpp\"\n\n\n";

	for (auto & ev : kmw4log_events)
	{
		kmw4log_events_cpp_file_out << "template<> const std::string log4kmw::Event<"
			                        << ev.second.first << ">::description_ = \"" << ev.second.second << "\";\n";
	}//for
}





void process_event_declarations(
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line)
{

	ceps::ast::Nodeset& ns = universe;

	auto events_sections = ns[all{ "events" }];
	for (auto events_section_ : events_sections)
	{
		auto events_section = events_section_["events"];
		for (Nodebase_ptr p : events_section.nodes())
		{
			if (p->kind() != Ast_node_kind::structdef)
				continue;
			string strct_name = name(as_struct_ref(p));
			if (kmw4log_event_names.find(strct_name) != kmw4log_event_names.end())
				throw std::runtime_error("Redeclaration of event: '" + strct_name + "'");
			kmw4log_event_names.insert(strct_name);
			kmw4log_events[strct_name].first = kmw4log_events_counter++;
			kmw4log_events[strct_name].second = ceps::ast::Nodeset{ p }[strct_name]["description"].as_str();

			if (DEBUG_MODE) std::cout << "kmw4log_event: " << strct_name << "description= \"" << kmw4log_events[strct_name].second << "\"" << std::endl;
		}//for
	}//for
}

void process_state_declarations(
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line)
{
	ceps::ast::Nodeset& ns = universe;

	auto all_states = ns[all{ "state" }];
	for (auto state_ : all_states)
	{
		auto state = state_["state"];
		auto const & id = state["id"];
		if (id.nodes().empty())
			throw std::runtime_error("No id found for state.\n");
		auto const & value = state["value"];		
		if (value.nodes().empty())
			throw std::runtime_error("No value found for state.\n");
		auto const & style_sheet = state["ceps_representation"]["file"];
		std::string state_id;
		std::string sysstate_kind = "Systemstate";
		if (!is_id_or_symbol(id.nodes()[0],sysstate_kind,state_id))
			throw std::runtime_error("Id for state should be an identifier or symbol of kind systemstate.\n");

		if (kmw4log_state_names.find(state_id) != kmw4log_state_names.end())
			throw std::runtime_error("Redefinition of \""+state_id+"\"");
		kmw4log_state_names.insert(state_id);

		kmw4log_states[state_id] = make_tuple(kmw4log_state_counter++,
												"",
												value.nodes(),
												 style_sheet.nodes().size()?style_sheet.nodes()[0]:nullptr );
		
		if (DEBUG_MODE) std::cout << "kmw4log_state: " << state_id /* << "description= \"" << kmw4log_events[strct_name].second << "\"" */ << std::endl;

	}//for
}


string escape_c_string(string const & in)
{
	string result;
	for (int i = 0; i != in.length(); ++i)
	{
		if (in[i] == '\"') result += "\\\"";
		else result += in[i];
	}//for
	return result;
}//escape_sql_string

void write_kmw4log_states_artifacts(
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line
	)
{
	if (kmw4log_states.size() == 0) return;

	std::string out_path = cmd_line.out_path;

	if (DEBUG_MODE) std::cout << "Outpath: " << out_path << std::endl;

	string kmw4log_states_hpp_out_name{ out_path + "log4kmw_states.hpp" };
	string kmw4log_states_cpp_out_name{ out_path + "log4kmw_states.cpp" };

	mk_directories(out_path);

	std::ofstream kmw4log_states_hpp_file_out{ kmw4log_states_hpp_out_name };
	std::ofstream kmw4log_states_cpp_file_out{ kmw4log_states_cpp_out_name };

	if (!kmw4log_states_hpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_states_hpp_out_name + "'\n");
	if (!kmw4log_states_cpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_states_cpp_out_name + "'\n");

	write_copyright_and_timestamp(kmw4log_states_hpp_file_out,"LOG4KMW_STATES.HPP",print_header_in_log4kmw_events_files);
	write_copyright_and_timestamp(kmw4log_states_cpp_file_out,"LOG4KMW_STATES.CPP",print_header_in_log4kmw_events_files);

	kmw4log_states_hpp_file_out << "#ifndef LOG4KMW_STATES_HPP_INC\n";
	kmw4log_states_hpp_file_out << "#define LOG4KMW_STATES_HPP_INC\n\n";
	kmw4log_states_hpp_file_out << "#include \"log4kmw_state.hpp\"\n#include \"log4kmw_dynamic_bitset.hpp\"\n#include<string>\n#include<thread>\n#include<mutex>\n\n";
	kmw4log_states_hpp_file_out << "namespace log4kmw_states{\n";
	kmw4log_states_hpp_file_out << " extern std::mutex global_lock;\n";

	
	kmw4log_states_cpp_file_out << "#include \"log4kmw_states.hpp\"\n\n\n";
	kmw4log_states_cpp_file_out << "std::mutex log4kmw_states::global_lock;\n";

	for (auto & ev : kmw4log_states)
	{
		//kmw4log_events_hpp_file_out << " typedef log4kmw::Event<" << ev.second.first << "> " << ev.first << ";\n";
		kmw4log_states_hpp_file_out << " extern log4kmw::State<";
		kmw4log_states_cpp_file_out << "log4kmw::State<";
		

		auto state_values = get<2>(ev.second);
		auto state_values_count = get<2>(ev.second).size();
		decltype(state_values_count) state_value_counter = 0;
		
		std::stringstream cpp_initializer;

		for (auto state_value : state_values)
		{
			bool last_element = (++state_value_counter == state_values_count);

			ceps::ast::Unit_rep val_unit{ ceps::ast::all_zero_unit() };
			std::string base_type = "int";
			if (state_value->kind() == ceps::ast::Ast_node_kind::string_literal)
			{
				base_type = "std::string";
			}
			else if (state_value->kind() == ceps::ast::Ast_node_kind::float_literal)
			{
				base_type = "double";
				val_unit = ceps::ast::unit(ceps::ast::as_double_ref(state_value));
			}
			else if (state_value->kind() == ceps::ast::Ast_node_kind::int_literal)
			{
				val_unit = ceps::ast::unit(ceps::ast::as_int_ref(state_value));
			} else if(state_value->kind() == ceps::ast::Ast_node_kind::structdef && ("Dynamic_bitset" == name(as_struct_ref(state_value))) ){
				base_type = "log4kmw::Dynamic_bitset";
			} else {
				std::stringstream ss;
				ss << *state_value;
				throw std::runtime_error("Unsupported state value:"+ss.str()+".\n");
			}


			if (val_unit == ceps::ast::all_zero_unit())
			{
				kmw4log_states_hpp_file_out << base_type;
				kmw4log_states_cpp_file_out << base_type;
			}
			else
			{
				kmw4log_states_hpp_file_out << "SI::Quantity<SI::Unit<";
				kmw4log_states_hpp_file_out << (int)val_unit.m << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.kg << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.s << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.ampere << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.kelvin << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.mol << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.candela << "> ";

				kmw4log_states_hpp_file_out << "," << base_type << "> ";

				kmw4log_states_cpp_file_out << "SI::Quantity<SI::Unit<";
				kmw4log_states_cpp_file_out << (int)val_unit.m << ",";
				kmw4log_states_cpp_file_out << (int)val_unit.kg << ",";
				kmw4log_states_cpp_file_out << (int)val_unit.s << ",";
				kmw4log_states_cpp_file_out << (int)val_unit.ampere << ",";
				kmw4log_states_cpp_file_out << (int)val_unit.kelvin << ",";
				kmw4log_states_cpp_file_out << (int)val_unit.mol << ",";
				kmw4log_states_cpp_file_out << (int)val_unit.candela << "> ";
				kmw4log_states_cpp_file_out << "," << base_type << "> ";
			}

			if (!last_element){
				kmw4log_states_hpp_file_out << ", ";
				kmw4log_states_cpp_file_out << ", ";
			}

			
			
			if (state_value->kind() == ceps::ast::Ast_node_kind::string_literal)
			{
				cpp_initializer << "\""
					<< escape_c_string(ceps::ast::value(ceps::ast::as_string_ref(state_value))) << "\"";
			}
			else if (state_value->kind() == ceps::ast::Ast_node_kind::float_literal)
			{
				cpp_initializer << ceps::ast::value(ceps::ast::as_double_ref(state_value));
			}
			else if (state_value->kind() == ceps::ast::Ast_node_kind::int_literal)
			{
				cpp_initializer << ceps::ast::value(ceps::ast::as_int_ref(state_value));
			} else if(state_value->kind() == ceps::ast::Ast_node_kind::structdef && "Dynamic_bitset" == name(as_struct_ref(state_value))){
				Nodeset ns(as_struct_ref(state_value).children());
				if (ns["size"].size() == 1) cpp_initializer << ns["size"].as_int();
				else throw std::runtime_error("Dynamic_bitset:Expected a size definition.\n");
			}
			if (!last_element) cpp_initializer << ", ";

			
		} //for (auto state_value : state_values)

		kmw4log_states_cpp_file_out << "> log4kmw_states::" << ev.first << "(";
		kmw4log_states_cpp_file_out << cpp_initializer.str();
		kmw4log_states_cpp_file_out << ");" << std::endl;


		kmw4log_states_hpp_file_out << "> " << ev.first << ";" << std::endl;
		
	}//for

	kmw4log_states_hpp_file_out << "\n";
	kmw4log_states_cpp_file_out << "\n";


	for (auto & ev : kmw4log_states)
	{
	
		kmw4log_states_hpp_file_out << " typedef log4kmw::State<";
		
		auto state_values = get<2>(ev.second);
		auto state_values_count = get<2>(ev.second).size();
		decltype(state_values_count) state_value_counter = 0;

		for (auto state_value : state_values)
		{

			bool last_element = (++state_value_counter == state_values_count);
			ceps::ast::Unit_rep val_unit{ ceps::ast::all_zero_unit() };
			std::string base_type = "int";
			if (state_value->kind() == ceps::ast::Ast_node_kind::string_literal)
			{
				base_type = "std::string";
			}
			else if (state_value->kind() == ceps::ast::Ast_node_kind::float_literal)
			{
				base_type = "double";
				val_unit = ceps::ast::unit(ceps::ast::as_double_ref(state_value));

			}
			else if (state_value->kind() == ceps::ast::Ast_node_kind::int_literal)
			{
				val_unit = ceps::ast::unit(ceps::ast::as_int_ref(state_value));
			} else if(state_value->kind() == ceps::ast::Ast_node_kind::structdef && ("Dynamic_bitset" == name(as_struct_ref(state_value))) ){
				base_type = "log4kmw::Dynamic_bitset";
			}

			if (val_unit == ceps::ast::all_zero_unit())
			{
				kmw4log_states_hpp_file_out << base_type;
			}
			else
			{
				kmw4log_states_hpp_file_out << "SI::Quantity<SI::Unit<";
				kmw4log_states_hpp_file_out << (int)val_unit.m << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.kg << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.s << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.ampere << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.kelvin << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.mol << ",";
				kmw4log_states_hpp_file_out << (int)val_unit.candela << "> ";

				kmw4log_states_hpp_file_out << "," << base_type << "> ";
			}
			if (!last_element) kmw4log_states_hpp_file_out << ", ";			
		}//for (states)
		kmw4log_states_hpp_file_out << "> " << ev.first << "_t;" << std::endl;
	}//for

	kmw4log_states_hpp_file_out << "\n"; 

	for (auto & ev : kmw4log_states)
	{
		kmw4log_states_hpp_file_out << " void write_state_" << ev.first << "(" << ev.first << "_t const &);\n";
		kmw4log_states_cpp_file_out << "void log4kmw_states::write_state_" << ev.first << "( log4kmw_states::" << ev.first << "_t const & v)\n";
		kmw4log_states_cpp_file_out << "{\n";
		kmw4log_states_cpp_file_out << "  " << "std::unique_lock<std::mutex> lock(global_lock)" << ";\n";
		kmw4log_states_cpp_file_out << "  " << ev.first << " = v;\n";
		kmw4log_states_cpp_file_out << "}\n";
		kmw4log_states_cpp_file_out << "\n";

		kmw4log_states_hpp_file_out << " " << ev.first << "_t  " << " read_state_" << ev.first << "(); \n";

		
		kmw4log_states_cpp_file_out << "log4kmw_states::" << ev.first << "_t  "  << " log4kmw_states::read_state_" << ev.first << "()\n";
		kmw4log_states_cpp_file_out << "{\n";
		kmw4log_states_cpp_file_out << "  " << "std::unique_lock<std::mutex> lock(global_lock)" << ";\n";
		kmw4log_states_cpp_file_out << "  return log4kmw_states::" << ev.first << ";\n";
		kmw4log_states_cpp_file_out << "}\n";

		kmw4log_states_hpp_file_out << " " << ev.first << "_t  " << " read_state_" << ev.first << "_no_lock(); \n";


		kmw4log_states_cpp_file_out << "log4kmw_states::" << ev.first << "_t  " << " log4kmw_states::read_state_" << ev.first << "_no_lock()\n";
		kmw4log_states_cpp_file_out << "{\n";
		kmw4log_states_cpp_file_out << "  return log4kmw_states::" << ev.first << ";\n";
		kmw4log_states_cpp_file_out << "}\n";

	}

	kmw4log_states_hpp_file_out << "}\n\n";
	kmw4log_states_hpp_file_out << "#endif\n";
}


void fetch_states(	ceps::ast::Nodeset & cur_rec ,
		            std::map<std::string,ceps::ast::Nodeset>& record_nodesets,
		            std::vector<std::string>& v,
		            std::string rec_name,
		            std::set<std::string>& m ){

	if (m.find(rec_name) != m.end())
		throw std::runtime_error("Circular definition detected: '" + rec_name + "'.\n");
	m.insert(rec_name);


	for (Nodebase_ptr p : cur_rec.nodes())
	{
		if (p->kind() != Ast_node_kind::identifier)
			continue;
		string state_name = name(ceps::ast::as_id_ref(p));
		if (kmw4log_state_names.find(state_name) == kmw4log_state_names.end()){
			if (record_nodesets.find(state_name) == record_nodesets.end())
			 throw std::runtime_error("State/Record '" + state_name + "' used in record declaration was never defined.\n");
			fetch_states(	record_nodesets[state_name] ,record_nodesets,v,state_name,m);
			continue;
		}
		v.push_back(state_name);
	}//for

}

void process_record_declarations(std::map<std::string,std::vector<std::string>>& record_content,
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line)
{
	ceps::ast::Nodeset& ns = universe;
	auto records = ns[all{ "record" }];
	if (records.size() == 0)
		return;


	//First pass over records: gather info
		std::map<std::string,ceps::ast::Nodeset> record_nodesets;
		std::set<std::string> rec_ids;
		for (auto record_ : records){
			auto record = record_["record"];
			if (record["id"].nodes().size() == 0 || record["id"].nodes()[0]->kind() != Ast_node_kind::identifier)
				throw std::runtime_error("Record without id found. Syntax: record{id{my_record;}; ... };\n");
			auto record_id = name(ceps::ast::as_id_ref(record["id"].nodes()[0]));
			if (rec_ids.find(record_id) != rec_ids.end())
				throw std::runtime_error("Redefinition of record (id='"+record_id+"').\n");
			record_nodesets[record_id] = record;
			rec_ids.insert(record_id);
		}

	std::string out_path = cmd_line.out_path;
	if (DEBUG_MODE) std::cout << "Outpath: " << out_path << std::endl;



	string kmw4log_record_hpp_out_name{ out_path + "log4kmw_records.hpp" };
	string kmw4log_record_cpp_out_name{ out_path + "log4kmw_records.cpp" };
	mk_directories(out_path);
	
	std::ofstream kmw4log_record_hpp_file_out{ kmw4log_record_hpp_out_name };
	std::ofstream kmw4log_record_cpp_file_out{ kmw4log_record_cpp_out_name };

	if (!kmw4log_record_hpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_record_hpp_out_name + "'\n");
	if (!kmw4log_record_cpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_record_cpp_out_name + "'\n");

	write_copyright_and_timestamp(kmw4log_record_hpp_file_out,"LOG4KMW_RECORDS.HPP",print_header_in_log4kmw_events_files);
	write_copyright_and_timestamp(kmw4log_record_cpp_file_out,"LOG4KMW_RECORDS.CPP",print_header_in_log4kmw_events_files);

	kmw4log_record_cpp_file_out << "\n#include \"log4kmw_records.hpp\"\n\n";

	kmw4log_record_hpp_file_out << "#ifndef LOG4KMW_RECORDS_HPP_INC\n";
	kmw4log_record_hpp_file_out << "#define LOG4KMW_RECORDS_HPP_INC\n\n";
	kmw4log_record_hpp_file_out << "#include \"log4kmw_state.hpp\"\n#include \"log4kmw_states.hpp\"\n#include \"log4kmw_events.hpp\"\n#include \"log4kmw_record.hpp\"\n#include<string>\n\n";
	kmw4log_record_hpp_file_out << "namespace log4kmw_records{\n";


	//std::map<std::string,std::vector<std::string>> record_content;

    for (auto record_ : records){
		auto record = record_["record"];
		auto record_id = name(ceps::ast::as_id_ref(record["id"].nodes()[0]));
		auto & rec = record_nodesets[record_id];

		std::vector<std::string> v_result;
		std::set<std::string> m;
		fetch_states(	rec ,record_nodesets,v_result,record_id,m);
		record_content[record_id] = v_result;
	}

	for (auto record_ : records)
	{		
		auto record = record_["record"];
		auto record_id = name(ceps::ast::as_id_ref(record["id"].nodes()[0]));

		kmw4log_record_hpp_file_out << " using " 
			<< record_id << "_t = log4kmw::State_record <\n";
		
		auto & content = record_content[record_id];
		size_t number_of_ids = content.size();
		/*for (Nodebase_ptr p : record.nodes())
			if (p->kind() == Ast_node_kind::identifier)
				++number_of_ids;*/
		auto id_counter = 0;
		for (auto state_name : content)
		{		
			bool last_elem = (++id_counter == number_of_ids);
			kmw4log_record_hpp_file_out <<  "   log4kmw_states::" << state_name << "_t";
			if (!last_elem)
				kmw4log_record_hpp_file_out << ",\n";
			else
				kmw4log_record_hpp_file_out << "\n";
		}//for

		kmw4log_record_hpp_file_out << " >;\n";

		kmw4log_record_hpp_file_out << "struct "<<record_id<<"{"<<record_id<<"_t rec;"<<record_id<<"_t& record(){return rec;} };\n\n";

		kmw4log_record_hpp_file_out << " void make_record_from_states(" << record_id << "& rec);\n\n";
		
		kmw4log_record_cpp_file_out << "void log4kmw_records::make_record_from_states(" << record_id << "& rec)\n";
		kmw4log_record_cpp_file_out << "{\n";
		kmw4log_record_cpp_file_out << " std::unique_lock<std::mutex> lock(log4kmw_states::global_lock);\n";
		kmw4log_record_cpp_file_out << " rec.record() = log4kmw_records::" << record_id << "_t(\n";


		id_counter = 0;

		for (auto state_name : content)
		{
			bool last_elem = (++id_counter == number_of_ids);
	
			kmw4log_record_cpp_file_out << "   log4kmw_states::read_state_" << state_name << "_no_lock()";
			if (!last_elem)
				kmw4log_record_cpp_file_out << ",\n";
			else
				kmw4log_record_cpp_file_out << "\n";
		}//for		

		kmw4log_record_cpp_file_out << " );";
		kmw4log_record_cpp_file_out << "\n}\n";

	}//for
	kmw4log_record_hpp_file_out << "}\n";
	kmw4log_record_hpp_file_out << "#endif\n";






}





void generate_logger_files(
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line)
{
	ceps::ast::Nodeset& ns = universe;
	auto records = ns[all{ "record" }];
	if (records.size() == 0)
		return;

	std::string out_path = cmd_line.out_path;
	if (DEBUG_MODE) std::cout << "Outpath: " << out_path << std::endl;



	string kmw4log_loggers_hpp_out_name{ out_path + "log4kmw_loggers.hpp" };
	string kmw4log_loggers_cpp_out_name{ out_path + "log4kmw_loggers.cpp" };
	string kmw4log_std_reader_cpp_out_name{ out_path + "log4kmw_default_log_reader.cpp" };

	mk_directories(out_path);

	std::ofstream kmw4log_loggers_hpp_file_out{ kmw4log_loggers_hpp_out_name };
	std::ofstream kmw4log_loggers_cpp_file_out{ kmw4log_loggers_cpp_out_name };
	std::ofstream kmw4log_std_reader_cpp_file_out{ kmw4log_std_reader_cpp_out_name };


	if (!kmw4log_loggers_hpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_loggers_hpp_out_name + "'\n");
	if (!kmw4log_loggers_cpp_file_out)
		throw std::runtime_error("Cannot create '" + kmw4log_loggers_cpp_out_name + "'\n");
	if (!kmw4log_std_reader_cpp_file_out)
			throw std::runtime_error("Cannot create '" + kmw4log_std_reader_cpp_out_name + "'\n");

	write_copyright_and_timestamp(kmw4log_loggers_hpp_file_out,"LOG4KMW_LOGGERS.HPP",print_header_in_log4kmw_events_files);
	write_copyright_and_timestamp(kmw4log_loggers_cpp_file_out,"LOG4KMW_LOGGERS.CPP",print_header_in_log4kmw_events_files);
	write_copyright_and_timestamp(kmw4log_std_reader_cpp_file_out,"LOG4KMW_DEFAULT_LOG_READER.CPP",print_header_in_log4kmw_events_files);

	kmw4log_loggers_cpp_file_out << "\n#include \"log4kmw_records.hpp\"\n#include \"log4kmw_loggers.hpp\"\n\n";

	kmw4log_loggers_hpp_file_out << "#ifndef LOG4KMW_LOGGERS_HPP_INC\n";
	kmw4log_loggers_hpp_file_out << "#define LOG4KMW_LOGGERS_HPP_INC\n\n";
	kmw4log_loggers_hpp_file_out << "#include \"log4kmw_state.hpp\"\n#include \"log4kmw_states.hpp\"\n"
                                 << "#include \"log4kmw_events.hpp\"\n#include \"log4kmw_record.hpp\"\n"
                                 << "#include \"log4kmw_logger.hpp\"\n#include \"log4kmw_events.hpp\"\n#include<string>"
								 << "\n#include <thread>\n#include <mutex>\n\n";
	kmw4log_loggers_hpp_file_out << "namespace log4kmw_loggers{\n";

	std::vector<std::string> loggers;


	for (auto record_ : records)
	{
		auto record = record_["record"];
		if (record["id"].nodes().size() == 0 || record["id"].nodes()[0]->kind() != Ast_node_kind::identifier)
			throw std::runtime_error("Record declarations require an id. Example: record{id{my_record;}; };\n");
		auto record_id = name(ceps::ast::as_id_ref(record["id"].nodes()[0]));
		loggers.push_back(record_id);
		/*
		kmw4log_loggers_hpp_file_out << " using logger_" << record_id << "_t = log4kmw::Logger<log4kmw_records::"
			<< record_id << ", log4kmw::persistence::memory_mapped_file>;\n";
		kmw4log_loggers_hpp_file_out << " extern log4kmw::Logger<log4kmw_records::" 
			<< record_id << ", log4kmw::persistence::memory_mapped_file>" << " logger_" << record_id << ";\n";
		 *
		 */

		gen_cpp_block g;

		indent(kmw4log_loggers_hpp_file_out) << "struct logger_" << record_id << "_t{"<<gen_cpp_logger_type(record_id)<<"log;\n";
		indent(kmw4log_loggers_hpp_file_out) << gen_cpp_logger_type(record_id) << "& logger(){return log;}" <<"};\n";
		//indent(kmw4log_loggers_hpp_file_out) << gen_cpp_logger_type(record_id) << "logger_"<<record_id << ";\n";
		indent(kmw4log_loggers_hpp_file_out) << "extern logger_" << record_id << "_t" << " logger_"<<record_id << ";\n";
		indent(kmw4log_loggers_cpp_file_out) << "log4kmw_loggers::logger_" << record_id << "_t" << " log4kmw_loggers::logger_"<<record_id << ";\n";




		/*kmw4log_loggers_cpp_file_out << "log4kmw::Logger<log4kmw_records::"
			<< record_id << ", log4kmw::persistence::memory_mapped_file> log4kmw_loggers::" << "logger_" << record_id << ";\n";*/
		kmw4log_loggers_hpp_file_out << " extern std::mutex " << "logger_mutex_" << record_id << ";\n";
		kmw4log_loggers_cpp_file_out << "std::mutex " << gen_cpp_loggers_nmspc() <<"logger_mutex_" << record_id << ";\n";
	}

	kmw4log_loggers_hpp_file_out << "\n\n";
	
	for (auto const & ev : kmw4log_events)
	{
		for (auto const & log : loggers)
		{
			std::string rec = log;
			kmw4log_loggers_hpp_file_out << " void log_event(" << "log4kmw_events::" << ev.first << " const &, logger_"
				<< log << "_t &)" << ";\n";

			kmw4log_loggers_cpp_file_out << " void "<< gen_cpp_loggers_nmspc() <<"log_event(" << "log4kmw_events::" << ev.first << " const & ev, "<<  gen_cpp_loggers_nmspc() <<"logger_"
				<< log << "_t & log)" << "\n";
			kmw4log_loggers_cpp_file_out << "{\n";
			kmw4log_loggers_cpp_file_out << " if(!log.logger()) throw std::runtime_error(\"Logger '" << log << "' invalid (not initialized?)\");\n";
			kmw4log_loggers_cpp_file_out << " log4kmw_records::" << rec << " rec;\n";
			kmw4log_loggers_cpp_file_out << " log4kmw_records::make_record_from_states(rec);\n";
			//std::unique_lock<std::mutex> lock(log4kmw_states::global_lock);
			kmw4log_loggers_cpp_file_out << " {\n";
			kmw4log_loggers_cpp_file_out << "  std::unique_lock<std::mutex> lock(log4kmw_loggers::logger_mutex_"<<log<<");\n";
			kmw4log_loggers_cpp_file_out << "  log.logger().append(rec.record());\n";
			kmw4log_loggers_cpp_file_out << " }\n";
			kmw4log_loggers_cpp_file_out << "}\n\n";
		}
	}

	
	kmw4log_loggers_hpp_file_out << "}//namespace log4kmw_loggers\n";
	kmw4log_loggers_hpp_file_out << "#endif\n";




	kmw4log_std_reader_cpp_file_out << "#include <iostream>\n";
	kmw4log_std_reader_cpp_file_out << "#include <type_traits>\n";
	kmw4log_std_reader_cpp_file_out << "#include <tuple>\n";
	kmw4log_std_reader_cpp_file_out << "#include <cstdint>\n";
	kmw4log_std_reader_cpp_file_out << "#include <memory>\n";
	kmw4log_std_reader_cpp_file_out << "#include <iterator>\n";
	kmw4log_std_reader_cpp_file_out << "#include <limits>\n";
	kmw4log_std_reader_cpp_file_out << "#include <cstdlib>\n";
	kmw4log_std_reader_cpp_file_out << "#include <time.h>\n";
	kmw4log_std_reader_cpp_file_out << "#include <sstream>\n";
	kmw4log_std_reader_cpp_file_out << "#include <vector>\n";
	kmw4log_std_reader_cpp_file_out << "#include <thread>\n";
	kmw4log_std_reader_cpp_file_out << "#include <chrono>\n";
	kmw4log_std_reader_cpp_file_out << "#include <random>\n";
	kmw4log_std_reader_cpp_file_out << "#include \"log4kmw_states.hpp\"\n";
	kmw4log_std_reader_cpp_file_out << "#include \"log4kmw_records.hpp\"\n";
	kmw4log_std_reader_cpp_file_out << "#include \"log4kmw_loggers.hpp\"\n";

	kmw4log_std_reader_cpp_file_out << R"(
#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#else
#endif
)";

	kmw4log_std_reader_cpp_file_out << "using namespace log4kmw;\n";

	kmw4log_std_reader_cpp_file_out <<
R"(
int main(int argc, char** argv)
{
 using namespace std;
 using namespace log4kmw;
 using namespace log4kmw_loggers;
 using namespace log4kmw_events;
 
 if(argc == 1)
 {
   cerr << "\n\nlog4kmw reader.\n\nUsage :" << argv[0] << " FILE [FILE...]\nWhere FILE is a logfile generated by a compatible logwriter.\n\n"; 
   return 1;
 }

 
 for (auto i = 1; i < argc; ++i)
 {
   struct stat sb = {0};
   if (-1 == stat(argv[i],&sb))
   {
     cerr << "\n***Fatal Error: Couldn't retrieve information about '"<< argv[i] << "'.\n\n";
     return 2;
   }
   logger_status_all.init(log4kmw::persistence::memory_mapped_file(argv[i], sb.st_size, false));
   int counter = 1;
   for (
		auto it = log4kmw_loggers::logger_status_all.cbegin(); // get iterator pointing to the very first entry in the logbook
		it != log4kmw_loggers::logger_status_all.cend();       // loop while we haven't reached the end
		++it		   
		)
	{
        std::cout << "LOG ENTRY #"<<counter++<<":\n"; 
		std::cout << (*it).states() << std::endl;
	}
   
 }
 

}
)";
}




void process_test(	ceps::Ceps_Environment& ceps_env,
		ceps::ast::Nodeset& universe,
		Result_process_cmd_line& cmd_line, ceps::ast::Nodeset ns, std::ostream& os, std::string record_id){

	std::string state;
	std::string event;
	std::string rec_kind;
	std::string id_or_sym;
	std::string func_name;

    std::string ev_kind = "Event";

	Nodebase_ptr ass_rhs;
	for(auto node : ns.nodes()){
		std::vector<ceps::ast::Nodebase_ptr> args;
		if (is_state_assignment(node,state,ass_rhs)){
			//std::cout << *ass_rhs << std::endl;
			indent(os) << gen_cpp_state_assignment(state,ass_rhs) << ";\n";
		} else if (is_id_or_symbol(node,ev_kind,event)){
			indent(os) << gen_cpp_log_event(event,"logger_"+record_id) << ";\n";
		} else if (is_method_call(node,id_or_sym,rec_kind,func_name,args)){
			//std::cout << *node << std::endl;
			//std::cout << id_or_sym <<"/"<<rec_kind << "."  << func_name << std::endl;
			indent(os) << gen_cpp_method_call(id_or_sym,rec_kind,func_name,args) << ";\n";
		} else if (is_func_call(node, func_name,args)){
			indent(os) << gen_cpp_func_call(func_name,args) << ";\n";
		}
	}
}


void gen_cpp_log_printers(std::ostream& os,
		kmw4log_states_t& all_states,
		std::map<std::string,std::vector<std::string>>& all_records,
	    ceps::Ceps_Environment& ceps_env,
	    ceps::ast::Nodeset& universe,
	    Result_process_cmd_line& cmd_line){


	os << R"(/*Start section "Log Printers"*/
namespace log4kmw_test{ namespace meta_info{)" << "\n";
	gen_cpp_block outer_block(&os,false);

	{
		indent(os) << "enum base_type{Float,Double,Int,String,Dynamicbitset};\n";
		indent(os) << "using state_name=std::string;using value_types_with_units=std::vector<base_type,int/*ceps::ast::Unit_rep*/>;" << "\n";

		for(auto& rec: all_records){
			indent(os) << "std::vector< std::pair<std::string,std::vector<std::pair<base_type,ceps::ast::Unit_rep>>>>"<<rec.first<< "()\n";
			gen_cpp_block func_block(&os,true);
			indent(os) << "return ";
			gen_cpp_block return_block(&os,true);return_block.rbr = "};";
			{
				gen_cpp_block b(&os,false);
				auto& state_ids = rec.second;
				for(size_t i = 0; i != state_ids.size();++i){
					 indent(os) << "std::make_pair(std::string(\"" << state_ids[i]<< "\"),std::vector<std::pair<base_type,ceps::ast::Unit_rep>>";
					 auto& svalues = std::get<2>(all_states[state_ids[i]]);
					 {gen_cpp_block b(&os,true);
					 for(size_t j = 0; j != svalues.size(); ++j  ){
						 auto n = svalues[j];
						 indent(os) << "std::make_pair(";
						 ceps::ast::Unit_rep u;
						 if (n->kind() == ceps::ast::Ast_node_kind::int_literal) {u = ceps::ast::unit(ceps::ast::as_int_ref(n)); os << "base_type::Int";}
						 else if (n->kind() == ceps::ast::Ast_node_kind::float_literal){u = ceps::ast::unit(ceps::ast::as_double_ref(n)); os << "base_type::Double";}
						 else if (n->kind() == ceps::ast::Ast_node_kind::string_literal) os << "base_type::String";
						 else if (n->kind() == ceps::ast::Ast_node_kind::structdef && ("Dynamic_bitset" == name(as_struct_ref(n))) ) os << "base_type::Dynamicbitset";
						 os <<",";
						 os << "ceps::ast::Unit_rep(";
						 os << gen_cpp_unit(u);
						 os << ")";
						 os <<")";
						 if (j+1 < svalues.size()) {os << ",\n";};
					 }}
					 indent(os) << ")";if (i+1 < state_ids.size()) {os << ",";};os << "\n";
				}
			}
		}
	}

	//Create default loggers
	for(auto& rec: all_records){
		indent(os) << "void log_print( log4kmw_loggers::logger_" << gen_cpp_record_type(rec.first) << "& logger)";
		gen_cpp_block b(&os,true);
		indent(os) << "for (auto it = logger.logger().cbegin(); it != logger.logger().cend();++it)\n";
		{
		 gen_cpp_block b(&os,true);
		 //indent(os) << "std::cout << \"!\" << std::endl;\n";
		 indent(os) <<  "std::cout << (*it).states() << std::endl;\n";
		 auto& state_ids = rec.second;
		 for(size_t i = 0; i != state_ids.size();++i){
			 //indent(os) << "\"" << state_ids[i]<< "\";\n";
		 }
		}
	}
	//Create ceps loggers
	for(auto& rec: all_records){
		indent(os) << "void log_print_ceps( log4kmw_loggers::logger_" << gen_cpp_record_type(rec.first) << "& logger)";
		gen_cpp_block b(&os,true);



		for(size_t i = 0; i != rec.second.size();++i){
			if (nullptr == std::get<3>(all_states[rec.second[i]])) continue;
			if ( std::get<2>(all_states[rec.second[i]]).size() == 1 &&
			      (std::get<2>(all_states[rec.second[i]])[0]->kind() == ceps::ast::Ast_node_kind::structdef &&
			    		 ("Dynamic_bitset" == name(as_struct_ref( std::get<2>(all_states[rec.second[i]])[0] ))) )
				)
			{
				indent(os) << "ceps::Ceps_Environment ceps_env;\nceps::ast::Nodeset universe"<<i<<";\n";
			}
			else fatal(-1, "Style sheets not supported for ... .\n" );
		    //std::cout << *std::get<3>(all_states[rec.second[i]]) << std::endl;
		}


		indent(os) << "for (auto it = logger.logger().cbegin(); it != logger.logger().cend();++it)\n";
		{
		 gen_cpp_block b(&os,true);
		 indent(os) << "std::cout << \"" << rec.first << "{\\n \";\n";

		 auto& state_ids = rec.second;
		 for(size_t i = 0; i != state_ids.size();++i){
			 indent(os) << "// Write " << state_ids[i]<< "\n";
			 indent(os) << "std::cout << \" " << state_ids[i] << "{\\n \";\n";

			 indent(os) << "auto state"<<i <<" = log4kmw::get_value<" << i <<">((*it).states());\n";

			 indent(os) << "std::cout << \" };\\n\";";
		 }
		 indent(os) << "std::cout << \"};\\n\";";
		}
	}

	os << R"(}}/*End section "Log Printers"*/)" << "\n";
}



void generate_tests( kmw4log_states_t& all_states,
		std::map<std::string,std::vector<std::string>>& all_records,
	ceps::Ceps_Environment& ceps_env,
	ceps::ast::Nodeset& universe,
	Result_process_cmd_line& cmd_line){

	ceps::ast::Nodeset& ns = universe;
	auto tests = ns[all{ "test" }];
	if (tests.size() == 0)
		return;
	std::string out_path = cmd_line.out_path;
	mk_directories(out_path);
	string kmw4log_loggers_test_cpp_out_name{ out_path + "log4kmw_loggers_tests.cpp" };
	std::ofstream kmw4log_loggers_test_cpp_out{ kmw4log_loggers_test_cpp_out_name };
	if (!kmw4log_loggers_test_cpp_out)
	 throw std::runtime_error("Cannot create '" + kmw4log_loggers_test_cpp_out_name + "'\n");
	write_copyright_and_timestamp(kmw4log_loggers_test_cpp_out,"LOG4KMW_LOGGERS_TESTS.CPP",print_header_in_log4kmw_events_files);
	kmw4log_loggers_test_cpp_out << gen_cpp_default_header_for_logger_client() << "\n";
	kmw4log_loggers_test_cpp_out << gen_cpp_default_header_for_meta_information() << "\n\n";

	indent(kmw4log_loggers_test_cpp_out) << gen_cpp_utils() << "\n";
	gen_cpp_log_printers(kmw4log_loggers_test_cpp_out,all_states,all_records,ceps_env,universe,cmd_line);
	std::set<std::string> test_names_seen;
	std::vector<std::string> test_procs;
	int counter = 0;
	for(auto test_:tests){
		++counter;
		std::string test_name;
		auto test = test_["test"];
		if (!ns_is_string(test["name"],test_name)){
			test_name = std::to_string(counter);
		}
		auto logfiledetails = test["logfile"];
		if (logfiledetails.size() == 0){
			std::cerr << "***Warning: logfile missing in test definition.\n";
			continue;
		}

		test_procs.push_back("test_"+test_name);
		std::string record_id;
		std::string logfile_path;
		int logfile_size_in_bytes;

		if (!ns_is_id_or_symbol(logfiledetails["record"],"Record",record_id))
			throw std::runtime_error("record definition in tests must contain a record-id\n");
		if (!ns_is_string(logfiledetails["path"],logfile_path))
			throw std::runtime_error("path definition in tests must contain a string\n");
		if (!ns_is_int(logfiledetails["size"],logfile_size_in_bytes))
			throw std::runtime_error("size definition in tests must contain an int\n");

		indent(kmw4log_loggers_test_cpp_out) << gen_cpp_test_run_signature(test_name);
		gen_cpp_block b1(&kmw4log_loggers_test_cpp_out,true);
		indent(kmw4log_loggers_test_cpp_out) << gen_cpp_init_logger(record_id,gen_cpp_mmapped_file(logfile_path,logfile_size_in_bytes)) << "\n";
		gen_cpp_block b2(&kmw4log_loggers_test_cpp_out,true);
		indent(kmw4log_loggers_test_cpp_out) << "using namespace log4kmw_test::meta_info;";
		indent(kmw4log_loggers_test_cpp_out) << "using namespace log4kmw_loggers;\n";
		indent(kmw4log_loggers_test_cpp_out) << "using namespace log4kmw_states;\n";
		process_test(ceps_env,universe,cmd_line, test,kmw4log_loggers_test_cpp_out,record_id);
	}

	{
		indent(kmw4log_loggers_test_cpp_out) << "void run_all_tests()";
		gen_cpp_block b1(&kmw4log_loggers_test_cpp_out,true);
		for(auto const& t : test_procs){
			indent(kmw4log_loggers_test_cpp_out) << t <<"();\n";
		}
	}
}


int main(int argc,char ** argv)
{
	if (argc <= 1)
	{
		cout << USAGE_MESSAGE;
		return EXIT_FAILURE;
	}	
	auto result_cmd_line = process_cmd_line(argc,argv);
	if (result_cmd_line.version_flag_set)
	{
			#ifdef __GNUC__
			 		std::cout << "\n"
						<< "VERSION " VERSION_DEF2CPP_MAJOR << "."<<  VERSION_DEF2CPP_MINOR   <<" (" __DATE__ << ") BUILT WITH GCC "<< "" __VERSION__ ""<< " on GNU/LINUX "
			 #ifdef __LP64__
							  << "64BIT"
			 #else
							  << "32BIT"
			 #endif
							  << "\nwritten by tomas.prerovsky@gmail.com\n" << std::endl;
			#else 
				#ifdef _MSC_FULL_VER
					std::cout << "\n"
						<< "VERSION " VERSION_DEF2CPP_MAJOR << "." << VERSION_DEF2CPP_MINOR << " (" __DATE__ << ") BUILT WITH MS VISUAL C++ " << _MSC_FULL_VER << " on Windows "
					#ifdef _WIN64
						<< "64BIT"
					#else
						<< "32BIT"
					#endif
						<< "\nwritten by tomas.prerovsky@gmail.com\n" << std::endl;
				#endif
			#endif
			std::cout << "ceps-core-libraries: " << ceps::get_version_info() << std::endl<< std::endl;
	}

	// Do sanity checks

	for(std::string const & f : result_cmd_line.definition_file_rel_paths)
	 if (!std::ifstream{f})
	 {
		 std::cerr << "\n***Error: Couldn't open file '" << f << "' " << std::endl << std::endl;
		 return EXIT_FAILURE;
	 }

	ceps::Ceps_Environment ceps_env{PRELUDE};
	ceps_env_current = &ceps_env;
	ceps::ast::Nodeset universe;
	current_universe = &universe;

	string last_file_processed;

	try{
		process_def_files(result_cmd_line.definition_file_rel_paths,ceps_env,last_file_processed,universe);
	}
	catch (ceps::interpreter::semantic_exception & se)
	{
		std::cerr << "***Error while evaluating file '" + last_file_processed + "':"<< se.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (std::runtime_error & re)
	{
		std::cerr << "***Error while evaluating file '" + last_file_processed + "':" << re.what() << std::endl;
		return EXIT_FAILURE;
	}

	try{
		process_event_declarations(ceps_env, universe, result_cmd_line);
		process_state_declarations(ceps_env, universe, result_cmd_line);
		write_kmw4log_events_artifacts(ceps_env, universe, result_cmd_line);
		write_kmw4log_states_artifacts(ceps_env, universe, result_cmd_line);
		process_record_declarations(log4kmw_record_contents,ceps_env, universe, result_cmd_line);
		generate_logger_files(ceps_env, universe, result_cmd_line);
		generate_tests(kmw4log_states,log4kmw_record_contents,ceps_env, universe, result_cmd_line);
	}
	catch (ceps::interpreter::semantic_exception & se)
	{
		std::cerr << "***Error: " << se.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (std::runtime_error & re)
	{
		std::cerr << "***Error: " << re.what() << std::endl;
		return EXIT_FAILURE;
	}

	

	return EXIT_SUCCESS;
}//main



