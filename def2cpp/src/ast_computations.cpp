/*
Copyright 2014, cpsdev (cepsdev@hotmail.com).
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of Google Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

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
*/

#include "ast_computations.hpp"

extern bool DONT_INCREMENT_SEQUENCER;

map<string, int> global_counter;

bool is_struct(ceps::ast::Nodebase* node)
{
	return node->kind() == Ast_node_kind::structdef;
}

bool is_test_precondition(ceps::ast::Struct & strct)
{
	return name(strct) == "Precondition";
}

Struct* expand_global_sequencer(Struct& attr_value)
{
	
	std::vector<Nodebase_ptr> new_values;
	for (auto const & e : attr_value.children())
	{
		if (e->kind() != Ast_node_kind::structdef)
		{
			new_values.push_back(e); continue;
		}

		auto it = global_counter.find(name(as_struct_ref(e)));

		if (it == global_counter.end())
		{
			new_values.push_back(e);
			continue;
		}
		int & ctr{ it->second };

		if(DONT_INCREMENT_SEQUENCER)
			new_values.push_back(new Int(ctr, all_zero_unit(), nullptr, nullptr, nullptr));
		else new_values.push_back(new Int(ctr++, all_zero_unit(), nullptr, nullptr, nullptr));

		delete e;
	}
	attr_value.children() = new_values;
	return &attr_value;
}


bool has_struct_defs(ceps::ast::Nodeset const & ns)
{
	for(Nodebase_ptr p : ns.nodes())
	{
		if (p->kind() == Ast_node_kind::structdef)
					return true;
	}//for
	return false;
}//has_struct_defs

string get_name_of_first_struct_def(ceps::ast::Nodeset const & ns)
{
	for(Nodebase_ptr p : ns.nodes())
	{
		if (p->kind() == Ast_node_kind::structdef)
			return name(as_struct_ref(p));
	}//for
	return "";
}

ceps::ast::Nodeset eval_path_expr(ceps::ast::Nodeset const & pattern,ceps::ast::Nodeset const & ns, std::string & path)
{
	ceps::ast::Nodeset result;
	ceps::ast::Nodeset rest_pattern = pattern;



	bool first_run = true;
	while(has_struct_defs(rest_pattern))
	{
		auto strct_name = get_name_of_first_struct_def(rest_pattern);
		if (first_run)
		{
			first_run = false;
			result = ns[strct_name];
		}
		else {
			path += ".";
			result = result[strct_name];
		}
		path += strct_name;
		rest_pattern = rest_pattern[first{strct_name}];

	}//while
	return result;
}//eval_path_expr


vector<string> get_attribute_names(ceps::ast::Nodeset const & ns)
{
	vector<string> result;
	set<string> already_read;

	for(Nodebase_ptr p : ns.nodes())
	{
		if (p->kind() == Ast_node_kind::structdef)
		{
			auto attr_name = name(as_struct_ref(p));
			//if (attr_name == "DONTCREATE") continue;
			if (already_read.find(attr_name) != already_read.end())
				continue;
			result.push_back(attr_name);
			already_read.insert(attr_name);
		}
	}//for
	return result;
}//get_attribute_names

vector<string> get_attribute_names(ceps::ast::Nodeset const & ns,ceps::ast::Nodeset const & ns2,set<string>& new_attributes)
{
	vector<string> result;
	set<string> already_read;

	for(Nodebase_ptr p : ns.nodes())
	{
		if (p->kind() == Ast_node_kind::structdef)
		{
			auto attr_name = name(as_struct_ref(p));
			//if (attr_name == "DONTCREATE") continue;
			if (already_read.find(attr_name) != already_read.end())
				continue;
			result.push_back(attr_name);
			already_read.insert(attr_name);
		}
	}//for

	for(Nodebase_ptr p : ns2.nodes())
	{
		if (p->kind() == Ast_node_kind::structdef)
		{
			auto attr_name = name(as_struct_ref(p));
			//if (attr_name == "DONTCREATE") continue;
			if (already_read.find(attr_name) != already_read.end())
				continue;
			result.push_back(attr_name);
			already_read.insert(attr_name);
			new_attributes.insert(attr_name);
		}
	}//for

	return result;
}//get_attribute_names

set<string>  get_attribute_names_as_set(ceps::ast::Nodeset const & ns)
{
	set<string> result;

	for(Nodebase_ptr p : ns.nodes())
	{
		if (p->kind() == Ast_node_kind::structdef)
		{
			auto attr_name = name(as_struct_ref(p));

			result.insert(attr_name);
		}
	}//for
	return result;
}//get_attribute_names_as_set



inline bool is_a_struct_def(Nodebase_ptr p)
{
	if (p == nullptr) return false;
	return p->kind() == Ast_node_kind::structdef;
}

inline bool is_a_struct_def(ceps::ast::Nodeset const & ns)
{
	if (ns.nodes().size() == 1) return is_a_struct_def(ns.nodes()[0]);
	return false;
}

bool empty_struct(ceps::ast::Nodeset const & struct_ns)
{
	if ( is_a_struct_def(struct_ns) && as_struct_ref(struct_ns.nodes()[0]).empty() )
		return true;
	return false;
}

ceps::ast::Int* copy_Int(Nodebase_ptr p)
{
	ceps::ast::Int & v = *dynamic_cast<ceps::ast::Int*>(p);
	return new ceps::ast::Int(value(v),unit(v),nullptr,nullptr,nullptr);
}

ceps::ast::Double* copy_Double(Nodebase_ptr p)
{
	 ceps::ast::Double & v = *dynamic_cast<ceps::ast::Double*>(p);
	 return new ceps::ast::Double( value(v), unit(v), nullptr, nullptr, nullptr);
}

ceps::ast::String* copy_String(Nodebase_ptr p)
{
	ceps::ast::String & v = *dynamic_cast<ceps::ast::String*>(p);
	return new ceps::ast::String( value(v), nullptr, nullptr, nullptr);
}

ceps::ast::Symbol* copy_Symbol(Nodebase_ptr p)
{
	 auto & v = as_symbol_ref(p);
	 return new ceps::ast::Symbol( name(v), kind(v), nullptr, nullptr, nullptr);
}



Nodebase_ptr deep_copy_struct(Struct* root,
		                 ceps::ast::Nodeset const & universe)
{
	//cout << "deep_copy_struct start" << endl;
	auto it = global_counter.find(name(as_struct_ref(root)));
	if (it != global_counter.end())
	{
		int & ctr{ it->second };
		if (DONT_INCREMENT_SEQUENCER) return new Int(ctr, all_zero_unit(), nullptr, nullptr, nullptr);
		else return new Int(ctr++, all_zero_unit(), nullptr, nullptr, nullptr);
	}
	Struct* result = new Struct(name(*root), nullptr, nullptr, nullptr);
	//Check first for an INHERIT Relation
	for(Nodebase_ptr p : root->children())
	{
		if (p->kind() == Ast_node_kind::structdef && name(as_struct_ref(p))=="INHERIT")
		{
			std::string path;
			ceps::ast::Nodeset default_values = eval_path_expr(Nodeset{p}["INHERIT"],universe,path);
			if (default_values.empty())
				warn(WARN_CANNOT_FIND_TEMPLATE, "'"+path+"'" );

			auto result_children = compute_instance_using_inheritance_relation(default_values,Nodeset{root}[name(*root)], universe);
			for(auto p : result_children.nodes())
				result->children().push_back(p);
			expand_global_sequencer(*result);
			return result;

		}
	}//for


	for(Nodebase_ptr p : root->children())
	{
		if (p->kind() == Ast_node_kind::structdef)
		{
			result->children().push_back(deep_copy_struct(as_struct_ptr(p), universe));
		}
		else if(p->kind() == Ast_node_kind::int_literal)
		{
			result->children().push_back(copy_Int(p));
		}
		else if (p->kind() == ceps::ast::Ast_node_kind::float_literal)
		{
			result->children().push_back(copy_Double(p));
		}
		else if (p->kind() == ceps::ast::Ast_node_kind::string_literal)
		{
			result->children().push_back(copy_String(p));
		}
		else if (p->kind() == ceps::ast::Ast_node_kind::symbol)
		{
			result->children().push_back(copy_Symbol(p));
		}
		else throw ceps::interpreter::semantic_exception{root,"Value of unknown type."};
	}//for

	return result;
}

bool contains_struct_defs(Struct* root)
{
	for(Nodebase_ptr p : root->children())
	{
		if (p->kind() == Ast_node_kind::structdef) return true;
	}
	return false;
}

Struct* contains_exactly_one_struct_def_and_nothing_else(Struct* root)
{
	if (root->children().size() != 1) return nullptr;

	Struct* r = nullptr;
	auto p = root->children()[0];  
	
	if (p->kind() == Ast_node_kind::structdef)
		return as_struct_ptr(p);
	return nullptr;
}

Struct* contains_exactly_one_child(Struct* root)
{
	if (root->children().size() != 1) return nullptr;
	return as_struct_ptr(root);
}

ceps::ast::Nodeset compute_instance_using_inheritance_relation(ceps::ast::Nodeset const & defaults_in,
																ceps::ast::Nodeset const & sub_ns,
																ceps::ast::Nodeset const & universe)
{
	//cout << "compute_instance_using_inheritance_relation" << endl;
	auto defaults = defaults_in; //instantiate_ast_fragment(defaults_in,universe);


	ceps::ast::Nodeset result;
	set<string> new_attributes;
	auto attributes_to_be_copied = get_attribute_names(defaults,sub_ns,new_attributes);

	map<string,bool> attr_deleted;
	for(auto const & attr : attributes_to_be_copied)
		attr_deleted[attr] = false;

	for(Nodebase_ptr p : sub_ns.nodes())
		{
			if (p->kind() != Ast_node_kind::structdef)
				continue;
			auto attr_name = name(as_struct_ref(p));
			if (attr_name == "DONTCREATE") continue;
			if(as_struct_ref(p).empty())
				attr_deleted[attr_name] = true;
		}//for

	for(auto const & attr : attributes_to_be_copied)
	{

		if(attr_deleted[attr]) continue;
		if (new_attributes.find(attr) != new_attributes.end())
		{
			//std::cout << attr << std::endl;
			//continue;
			if (attr == "INHERIT") continue;
			auto defs = sub_ns[all{attr}];
			for(size_t i = 0; i < defs.nodes().size();++i)
			{
				result.nodes().push_back(deep_copy_struct( as_struct_ptr(defs.nodes()[i]), universe ));
			}//for

			continue;
		}
		auto redefs = sub_ns[all{attr}] ;
		if (redefs.size() == 0)
		{

			auto ns_defs = defaults[all{attr}];
			for(size_t i = 0; i < ns_defs.nodes().size();++i)
				result.nodes().push_back(deep_copy_struct( as_struct_ptr(ns_defs.nodes()[i]), universe ) );


		}
		else
		{
			auto ns_default_ = defaults[all{attr}];
			Nodeset ns_default;

			for (auto p : ns_default_.nodes())
			{
				ns_default.nodes().push_back(deep_copy_struct( as_struct_ptr(p), universe ));
			}

			//std::cout << "!" << ns_default << "!" << std::endl;
			if (!contains_struct_defs(as_struct_ptr(ns_default.nodes()[0])))
			{
				//std::cerr << "A" << std::endl;
				for(size_t i = 0; i < redefs.nodes().size();++i)
				{
					result.nodes().push_back(deep_copy_struct( as_struct_ptr(redefs.nodes()[i]), universe ) );
				}
				//std::cerr << "B" << std::endl;
			}
			else
			{
				//std::cerr << "C" << std::endl;
				for(size_t i = 0; i < redefs.nodes().size();++i)
				{
				 // std::cout << ns_default[0] << std::endl;
				 // std::cout << Nodeset{redefs.nodes()[i]} << std::endl;
				  auto v = compute_instance_using_inheritance_relation(Nodeset{as_struct_ptr(ns_default.nodes()[0])->children()},
						                                               Nodeset{as_struct_ptr(redefs.nodes()[i])->children()}, universe);
				  auto vv = new Struct(attr, nullptr, nullptr, nullptr);


				  for(size_t i = 0; i < v.nodes().size();++i)
				   vv->children().push_back(v.nodes()[i]);

				  result.nodes().push_back(vv);
				}
				//std::cerr << "D" << std::endl;
			}
			//result.nodes().push_back(new Struct{attr});
		}
	}//for

	return result;
}//compute_instance_using_inheritance_relation



ceps::ast::Nodeset instantiate_ast_fragment(ceps::ast::Nodeset const & to_instantiate,ceps::ast::Nodeset const & universe)
{
	ceps::ast::Nodeset result;
	//cout << "instantiate_ast_fragment" << endl;

	if (to_instantiate["INHERIT"].size() == 0)
	{
		//cout << "A" << endl;
		Struct str("",nullptr,nullptr,nullptr);
		str.children().insert(str.children().end(),to_instantiate.nodes().begin(),to_instantiate.nodes().end());
		auto r = deep_copy_struct(&str,universe);
		result.nodes().insert(result.nodes().end(),as_struct_ptr(r)->children().begin(),as_struct_ptr(r)->children().end() );
		str.children().clear();
		//cout << "B" << endl;
		return result;
	}
	std::string path;
	ceps::ast::Nodeset default_values = eval_path_expr(to_instantiate["INHERIT"],universe,path);
	if (default_values.empty()) warn(WARN_CANNOT_FIND_TEMPLATE, "'" + path + "'");
	else default_values = instantiate_ast_fragment(default_values, universe);




	Struct str_temp("",nullptr,nullptr,nullptr);
	//str.children().insert(str.children().end(),to_instantiate.nodes().begin(),to_instantiate.nodes().end());
	for(auto pp : to_instantiate.nodes())
	{
		if (pp->kind() == Ast_node_kind::structdef)
		 if(name(as_struct_ref(pp)) == "INHERIT") continue;
		str_temp.children().push_back(pp);
	}
	auto r = deep_copy_struct(&str_temp,universe);

	ceps::ast::Nodeset to_instantiate_temp;
	to_instantiate_temp.nodes().insert(to_instantiate_temp.nodes().end(),as_struct_ptr(r)->children().begin(),as_struct_ptr(r)->children().end() );
	str_temp.children().clear();


	result = compute_instance_using_inheritance_relation(default_values,to_instantiate_temp, universe);

	return result;
}
