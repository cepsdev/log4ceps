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

#include "report_generator.hpp"
#include <iostream>
#include <map>
#include <stdlib.h>
#include <fstream>
#include <sstream>

#include "cmdline_utils.hpp"
#include "ast_computations.hpp"

std::string inherit_path(ceps::ast::Nodeset const & path)
{
	using namespace std;
	string result;
	auto rest_pattern = path;

	while(has_struct_defs(rest_pattern))
	{
		auto strct_name = get_name_of_first_struct_def(rest_pattern);
		result = result + strct_name;
		rest_pattern = rest_pattern[strct_name];
		if (has_struct_defs(rest_pattern))
			result = result + ".";

	}//while

	return result;
}
void compute_attr2values_internal(vector< pair<string,string>  >& a2v, ceps::ast::Nodeset const & ns,std::string const & name)
{
	using namespace std;
	using namespace ceps::ast;

	if (ns.nodes().size() == 1 && !has_struct_defs(ns))
	{
		auto attr = ns.nodes()[0];

		if (attr->kind() == Ast_node_kind::string_literal)
		{
				a2v.push_back( pair<string,string>(name, "\"" + value(as_string_ref(attr)) + "\""));
		}
		else if (attr->kind() == Ast_node_kind::float_literal)
		{
			std::stringstream s;

			s << std::setprecision(12) << value(as_double_ref(attr)) ;
			a2v.push_back( pair<string,string>(name,s.str()) );
		}
		else if (attr->kind() == Ast_node_kind::int_literal)
		{
			std::stringstream s;
			s <<  value(as_int_ref(attr)) ;
			a2v.push_back( pair<string,string>(name,s.str()));
		}
		else a2v.push_back( pair<string,string>(name, "<b>null</b>"));

		return;
	}
	for(Nodebase_ptr p :ns.nodes())
	{
		if (p->kind() != Ast_node_kind::structdef) continue;
		string strct_name = ceps::ast::name(as_struct_ref(p));

		if (strct_name == "Testcase") continue;
		if (strct_name == "__fileinfo_output_dir") continue;
		if (strct_name == "INHERIT") continue;
		compute_attr2values_internal( a2v, ceps::ast::Nodeset{as_struct_ref(p).children()},name+"."+strct_name );

	}
}

void compute_attr2values(vector< pair<string,string>  >& a2v, ceps::ast::Nodeset const & ns)
{
	for(Nodebase_ptr p :ns.nodes())
	{
		if (p->kind() != Ast_node_kind::structdef)
								continue;
		string strct_name = name(as_struct_ref(p));
		if (strct_name == "Testcase") continue;
		if (strct_name == "__fileinfo_output_dir") continue;
		if (strct_name == "INHERIT") continue;
		compute_attr2values_internal( a2v, ceps::ast::Nodeset{as_struct_ref(p).children()},strct_name );

	}
}
extern void xml_write_value(std::ofstream & xml_file_out, Nodebase_ptr p);

void tagger(ofstream& os,ceps::ast::Nodeset const & ns )
{
	for(auto & e : ns.nodes() )
	{
		if (is_a_struct(e))
		{
			auto tag = name(as_struct_ref(e));
			os << "<" << tag << ">";
			tagger(os,Nodeset{e}[tag]);
			os << "</" << tag << ">";
		}
		else xml_write_value(os,e);
	}
}


void make_user_defined_reports(ceps::ast::Nodeset const & universe)
{

	auto reports = universe[all{"Report"}];
	for(auto const & report_ : reports)
	{
		auto report = report_["Report"];
		string title = report["title"].as_str();
		string filename = get_report_out_dir()+title+".html";
		ofstream f{filename};
		tagger(f,report["content"]);
	}
};

void make_html_report(ceps::ast::Nodeset const & universe)
{
	using namespace std;
	using namespace ceps::ast;

	make_user_defined_reports(universe);

	ofstream f{get_report_out_dir()+"index.html"};

	f << R"(

<!DOCTYPE html>
	<html>
	<head>
		<meta charset="utf-8">
		<title>Test Data Generation Report</title>
		<style>
			body {
				font: 80% arial, helvetica, sans-serif;
				margin: 0;
			}
			
			h1, h2 {
				margin: 0;
			}
			
			#header {
				background: #ccc;
			}
			
			#navigation {
				float: left;
				width: 150px;
			}
			
			#content {
				margin-left: 150px;
			}
			
			#footer {
				clear: left;
				background: #ccc;
			}
			
			#footer p {
				margin: 0;
			}
			
		</style>
	</head>

	<body>

		<div id="header">
			<h1>Test Data Generation Report</h1>
		</div>

		<!--div id="navigation">

			<ul>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
			</ul>

			<ul>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
				<li><a href=""></a></li>
			</ul>

	
		</div-->

		<div id="content">
)";


	
f << R"(		</div><p></p><p></p>
)";

	auto preconditions = universe[all{"Precondition"}];
	for(auto pre_ : preconditions)
	{
		auto pre = pre_["Precondition"];
		f << "<div align=\"center\" width = \"90%\" padding=\"10px\">";
		f << "<h2>";
		f << pre["Testcase"]["Title"].as_str();
		f << "(Id = " << pre["Testcase"]["Id"].as_str();
		f << ")</h2>";

		for(Nodebase_ptr p :pre.nodes())
		{
			if (p->kind() != Ast_node_kind::structdef)
						continue;
			string strct_name = name(as_struct_ref(p));
			if (strct_name == "Testcase") continue;
			if (strct_name == "__fileinfo_output_dir") continue;


			auto strct_as_ns = Nodeset{as_struct_ref(p).children()};

			f<<"<table border='1' width='70%'>";
			f<<"<tr><th colspan = '2'>"<< strct_name <<"</th><tr>";

			if (strct_as_ns["INHERIT"].size())
			{
				f << "<tr>";
				f << "<td>";
				f << "Inherits values of";
				f << "</td>";
				f << "<td>";
				f << "<i>" <<  inherit_path(strct_as_ns["INHERIT"]) << "</i>";
				f << "</col>";
				f << "</tr>";
			}
			vector< pair<string,string>  > attr2value;
			compute_attr2values(attr2value, strct_as_ns);

			for(auto const & v : attr2value)
			{
				f << "<tr>";
				f << "<td>";
				f << v.first;
				f << "</td>";
				f << "<td>";
				f << "<i>" <<  v.second << "</i>";
				f << "</col>";
				f << "</tr>";
			}


			f<<"</table><br/><br/>";
		}



		f << "</div>";
	}

f <<R"(
		<div id="footer">

			<p>)";

	time_t timer;
	time(&timer);
	tm * timeinfo;
	timeinfo = localtime (&timer);

	f	<< "Created "<< asctime(timeinfo) << "<br/>"
		<< "Generated with tst2db (c) the authors of tst2db.<br/>\n";


f << R"(</p>


		</div>

	</body>

	</html>
)" << endl;




}
