//#include "Common_Backend.h"
#include "SettingsVals.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>

using namespace LUNOBackend;

SettingsVals::SettingsVals()
{

}

SettingsVals::SettingsVals(std::string fileName, std::string iniFile)
{
	init(fileName, iniFile);
}

SettingsVals &LUNOBackend::SettingsVals::operator=(const SettingsVals &k)
{
	if (this != &k)
	{
		m_vals=k.m_vals;
	}

	return *this;
}

bool SettingsVals::init(std::string fileName, std::string iniFile)
{
	bool rc = false;

	m_iniFile = iniFile;

	if (fileName.size() > 0)
	{
		rc = true;

		rc = readSettings(fileName);
		readIni(1);
		generateArrays();
		readIni();
	}

	return rc;
}

bool SettingsVals::readSettings(std::string fileName)
{
	bool retVal = false;
	m_vals.clear();

	// check if file exists and can be opened
	std::ifstream inFile(fileName, std::ifstream::in);

	if (inFile.is_open())
	{
		// read line by line
		type_Vals actVal;
		actVal.key = "UNKNOWNXYZABC";
		actVal.descr = actVal.key;
		actVal.unit = "NA";
		actVal.group = "other";
		actVal.min = -DBL_MAX; // -std::numeric_limits<double>::max();
		actVal.max = DBL_MAX; // std::numeric_limits<double>::max();
		actVal.index1 = -1;
		actVal.index2 = -1;
		actVal.displayHint = "";
		actVal.help = "";
		actVal.arraySize1 = -1;
		actVal.arraySize2 = -1;
		actVal.defaultVal = "";
		actVal.accessLevel = 0;
		actVal.visible = true;

		std::string line;
		while (!inFile.eof())
		{
			retVal = true;

			getline(inFile, line);

			// replace leading spaces
			line = deleteLeadingSpaces(line);

			// check for new val
			if ((line.size() > 2) && (line.at(0) == '[') && (line.find(']') > 1))
			{
				// save actual value
				if (actVal.key != "UNKNOWNXYZABC")
				{
					if (actVal.descr == "UNKNOWNXYZABC")
						actVal.descr = actVal.key;
					m_vals.push_back(actVal);
				}

				// generate new value
				actVal.key = deleteAllChars(line, "[]");
				actVal.descr = actVal.key;
				actVal.group = "other";
				actVal.min = -DBL_MAX; // -std::numeric_limits<double>::max();
				actVal.max = DBL_MAX; // std::numeric_limits<double>::max();
				actVal.index1 = -1;
				actVal.index2 = -1;
				actVal.displayHint = "";
				actVal.help = "";
				actVal.arraySize1 = -1;
				actVal.arraySize2 = -1;
				actVal.defaultVal = "";
				actVal.accessLevel = 0;
				actVal.visible = true;
			}

			// check line for keywords...
			std::string left;
			std::string right;
			if (split(line, '=', left, right) && (!left.empty()) && (!right.empty()))
			{
				if (actVal.key != "UNKNOWNXYZABC")
				{
					right = deleteAllChars(right, "=\n\r\t");
					put(actVal, left, right);
				}
			}
		}

		// save last value
		if (actVal.key != "UNKNOWNXYZABC")
		{
			if (actVal.descr == "UNKNOWNXYZABC")
				actVal.descr = actVal.key;

			m_vals.push_back(actVal);
		}
	}

	return retVal;
}





// helpers
std::string SettingsVals::deleteLeadingSpaces(std::string in)
{
	std::string retVal = in;
	int i = 0;

	while (i < in.length() && (in.at(i++) == ' '));
	i--;

	if ((i > 0) && (i < in.length()))
		retVal = in.substr(i);

	return retVal;
}

std::string SettingsVals::deleteComment(std::string in)
{
	// delete everythiung after // 
	int pos = in.find("//");

	if (pos > 0)
		return in.substr(0, pos);

	return in;
}
std::string SettingsVals::deleteAllSpaces(std::string in)
{
	std::string retVal = "";
	for (std::string::iterator it = in.begin(); it != in.end(); it++)
		if (*it != ' ')
			retVal += *it;

	return retVal;
}

std::string SettingsVals::deleteAllChars(std::string in, std::string characters)
{
	std::string retVal = "";
	for (int i = 0; i < in.length(); i++)
	{
		if (characters.find(in.at(i)) == std::string::npos)
			retVal += in.at(i);
	}
	return retVal;

}

bool SettingsVals::split(std::string in, char seperator, std::string &left, std::string &right)
{
	int find = in.find(seperator);
	bool retVal = find > 0;

	if (retVal)
	{
		left = in.substr(0, find);
		if (find < in.length() - 1)
			right = in.substr(find + 1);
		else
			right = "";
	}

	return retVal;
}

bool SettingsVals::isConstant(std::string in)
{
	// check first character
	if ((int)in.length() <= 0)
		return true;

	if ((in.at(0) >= '0') && (in.at(0) <= '9'))
		return true;

	if ((in.at(0) == '+') || (in.at(0) == '-') || (in.at(0) == '.') || (in.at(0) == ','))
		return true;

	return false;
}

double SettingsVals::evalMinMax(std::string in)
{
	double retVal = -1;

	if ((in.find("_min(") != 0) && (in.find("_max(") != 0))
		return retVal;


	bool isMin = (in.find("_min(") == 0);
	std::string arg = in.substr(5);

	if ((int)arg.find(")") > 0)
	{
		arg = arg.substr(0, arg.find(")"));
	}

	std::string left;
	std::string right;

	double arg1 = 0;
	double arg2 = 0;

	if (split(arg, ',', left, right))
	{
		// at this momemnt left and right may not bes signed as variable becaus is within
		// an array argument. Just check again...

		if ((left.size()>0) && (left.at(0)!='_') && (!isConstant(left)))
			left = "_" + left;

		if ((right.size() > 0) && (right.at(0) != '_') && (!isConstant(right)))
			right = "_" + right;

		if (left.at(0) == '_') // variable
			getValDouble(left.substr(1), arg1);
		else
			arg1 = stod(replaceDecimalComma(left));

		if (right.at(0) == '_') // variable
			getValDouble(right.substr(1), arg2);
		else
			arg2 = stod(replaceDecimalComma(right));
	}

	retVal = arg1;
	if (isMin)
	{
		if (arg2 < arg1) retVal = arg2;
	}
	else
	{
		if (arg2 > arg1) retVal = arg2;
	}

	return retVal;
}

void SettingsVals::evaluate(type_parse &in)
{
	// calc arraySizes
	in.arraySize1 = -1;
	in.arraySize2 = -1;

	// call AFTER reading all vals resp when changing vals
	if (in.function == "array")
	{

		double res = -1;

		for (int i = 0; i < in.arguments.size(); i++)
		{
			std::string arg = in.arguments.at(i);

			// may a min/max function a variable or a constant
			if (((int)arg.size()>0) && ((arg.find("_min(") == 0) || (arg.find("_max(") == 0)))
			{
				res = evalMinMax(arg);
			}
			else // not min/max -> get variable or constant
			{
				if (((int)arg.size() > 0) && (arg.at(0) == '_')) // variable
				{
					getValDouble(arg.substr(1), res);
				}
				else
				{
					if ((int)arg.size() > 0)
						res = stod(replaceDecimalComma(arg));
				}
			}

			if (res>0)
			{
				if (i == 0) in.arraySize1 = (int)round(res);
				if (i == 1) in.arraySize2 = (int)round(res);
			}
		}
	}

	if ((in.function == "min") || (in.function == "max"))
	{
		bool isMin = (in.function == "min");

		double res = -1;

		for (int i = 0; i < in.arguments.size(); i++)
		{
			std::string arg = in.arguments.at(i);
			double act;
			
			if (arg.at(0) == '_') // variable
				getValDouble(arg.substr(1), act);
			else
				act = stod(replaceDecimalComma(arg));

			if (isMin)
			{
				if ((act < res) || (i == 0))
					res = act;
			}
			else
			{
				if ((act > res) || (i == 0))
					res = act;
			}
		}

		in.result = res;
	}


}

type_parse SettingsVals::parse(std::string in)
{
	type_parse retVal;

	std::string p = deleteAllSpaces(in);

	// replace brackets [ into ( and ] into )
	for (int i=0; i<p.length(); i++)
	{
		if (p.at(i) == '[')
			p[i] = '(';

		if (p.at(i) == ']')
			p[i] = ')';
	}
	std::transform(p.begin(), p.end(), p.begin(), ::tolower);

	// check for keywords
// array
	if ((int)p.find("array(") >= 0)
	{
		p = p.substr(p.find("array(") + 6);

		// search index
		while ((int)p.find(")") > 0)
		{
			std::string indexStr;
			retVal.function = "array";

			bool found = false;
			for (std::string::iterator it = p.begin(); (it != p.end()) && (!found); it++)
			{
				if (*it == ')')
				{
					found = true;
				}
				else
				{
					indexStr += *it;
				}
			}

			if (found)
			{
				if (!isConstant(indexStr))
					indexStr = '_' + indexStr;

				retVal.arguments.push_back(indexStr);

				p = p.substr(p.find(')') + 1);

				if ((int)p.find('(') == 0)
				{
					p = p.substr(1);
				}
			}
		}

		// search the rest for WITH or other identifier
		if ((int)p.find("with") == 0)
		{
			p = p.substr(4);
			while ((int)p.find(",") > 0)
			{
				retVal.arrayID.push_back(p.substr(0, p.find(",")));
				p = p.substr(p.find(",") + 1);
			}

			if ((int)p.length() > 0)
				retVal.arrayID.push_back(p);
			return retVal;
		}

		// search rest for displayHints 
		if ((int)p.length() > 0)
		{
			// ToDo: hier muss ich im Original-String suchen, wegen Groß/Klein Schreibung!
			if (p.find("|") > 0)
			{
				// search for beginning space
				int i = in.find("|");
				while (i > 0 && in.at(i) != ' ')
					i--;

				p = deleteLeadingSpaces(in.substr(i));
			}

			while ((int)p.find("|") > 0)
			{
				retVal.arrayID.push_back(deleteLeadingSpaces(deleteAllChars(p.substr(0, p.find("|")), ",")));
				p = p.substr(p.find("|") + 1);
			}

			if ((int)p.length() > 0)
				retVal.arrayID.push_back(deleteLeadingSpaces(deleteAllChars(p, ",")));

		}
		return retVal;
	}

	// min and max
	bool isMinMax = false;
	if ((int)p.find("min(") >= 0)
	{
		isMinMax = true;
		p = p.substr(p.find("min(") + 4);
		retVal.function = "min";
	}

	if ((int)p.find("max(") >= 0)
	{
		isMinMax = true;
		p = p.substr(p.find("max(") + 4);
		retVal.function = "max";
	}

	if (isMinMax)
	{
		if ((int)p.find(")") > 0)
		{
			p = p.substr(0, p.find(")"));
			std::string left;
			std::string right;

			split(p, ',', left, right);

			if (!isConstant(left))
				left = '_' + left;

			if (!isConstant(right))
				right = '_' + right;

			retVal.arguments.push_back(left);
			retVal.arguments.push_back(right);

			return retVal;
		}
	}

	
	// enum
	if (p.find("enum(") >= 0)
	{
		retVal.function = "enum";

		// take the original string becuse of lower and upper cases and spaces!
		p = in.substr(in.find("(")+1);

		if ((int)p.find_last_of(")") >= 0)
		{
			p = p.substr(0, p.find_last_of(")"));

			while ((int)p.find(',') > 0)
			{
				std::string e = p.substr(0, p.find(','));
				std::string left;
				std::string right;

				if (split(e, ':', left, right))
				{
					left = deleteLeadingSpaces(left);
					right = deleteLeadingSpaces(right);

					if ((int)left.length() > 0)
					{
						retVal.enumID.push_back(stoi(left));
						retVal.arguments.push_back(right);
					}
				}

				p = p.substr(p.find(',') + 1);
			}

			if ((int)p.length() > 0)
			{
				std::string left;
				std::string right;

				if (split(p, ':', left, right))
				{
					if ((int)left.length() > 0)
					{
						left = deleteLeadingSpaces(left);
						right = deleteLeadingSpaces(right);

						retVal.enumID.push_back(stoi(left));
						retVal.arguments.push_back(right);
					}
				}
			}
		}
		return retVal;
	}
	return retVal;
}

std::string SettingsVals::replaceDecimalComma(std::string in)
{
	std::string retVal = "";
	for (std::string::iterator it = in.begin(); it != in.end(); it++)
		if (*it != ',')
			retVal += *it;
		else
			retVal += '.';

	return retVal;

}

std::string SettingsVals::getIndex(std::string in, int &index1, int &index2)
{
	index1 = -1;
	index2 = -1;
	int posStart = (int)in.find('[');
	int posEnd = (int)in.find_last_of(']');

	std::string retVal = in;

	if ((posStart > 0) && (posEnd > 0) && (posEnd > (posStart + 1)))
	{
		std::string indexStr = in.substr(posStart + 1, posEnd - posStart - 1);
		retVal = in.substr(0, posStart);

		deleteAllChars(indexStr, " -+.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\t\n\r");

		if ((int)indexStr.find("][") >= 0) // two indices
		{
			std::string left;
			std::string right;

			split(indexStr, ']', left, right);
			right = right.substr(1);

			index1 = stoi(left);
			index2 = stoi(right);
		}
		else
		{

			index1 = stoi(indexStr);
		}
	}
	return retVal;
}

bool SettingsVals::put(type_Vals &val, std::string member, std::string value)
{
	std::string m = deleteAllSpaces(member);
	boost::to_lower(m);
	bool retVal = false;

	if (m == "key")
	{
		retVal = true;
		val.key = value;
	}

	if (m == "descr")
	{
		retVal = true;
		val.descr = value;
	}

	if (m == "unit")
	{
		retVal = true;
		val.unit = value;
	}

	if (m == "type")
	{
		retVal = true;
		val.type = value;
	}

	if (m == "value")
	{
		retVal = true;
		val.value = value;
	}

	if (m == "group")
	{
		retVal = true;
		val.group = value;
	}

	if (m == "tooltip")
	{
		retVal = true;
		val.toolTip = value;
	}

	if (m == "imagepath")
	{
		retVal = true;
		val.imagePath = value;
	}

	if (m == "help")
	{
		retVal = true;
		val.help = value;
	}

	if (m == "defaultval")
	{
		retVal = true;
		val.defaultVal = value;
	}

	if (m == "displayhint")
	{
		retVal = true;
		val.displayHint = value;
	}

	if (m == "min")
	{
		retVal = true;
		val.minStr = value;

		if ((value.size() > 0) && (isConstant(value)))
			val.min = stod(value);
		else
			val.min = -DBL_MAX;
	}

	if (m == "max")
	{
		retVal = true;
		val.maxStr = value;

		if ((value.size() > 0) && (isConstant(value)))
			val.max = stod(value);
		else
			val.max = DBL_MAX;
	}

	if (m == "index1")
	{
		retVal = true;
		std::string r = replaceDecimalComma(value);
		int d = std::stoi(r);

		val.index1 = d;
	}

	if (m == "index2")
	{
		retVal = true;
		std::string r = replaceDecimalComma(value);
		int d = std::stoi(r);

		val.index2 = d;
	}

	if (m == "accesslevel")
	{
		retVal = true;
		val.accessLevel = stoi(value);
	}

	if (m == "visible")
	{
		retVal = true;
		val.visible = value.find("1") > 0;
	}

	return retVal;
}

bool SettingsVals::get(type_Vals val, std::string member, std::string &value)
{
	std::string m = deleteAllSpaces(member);
	boost::to_lower(m);
	bool retVal = false;

	if (m == "key")
	{
		retVal = true;
		value = val.key;
	}

	if (m == "descr")
	{
		retVal = true;
		value = val.descr;
	}

	if (m == "type")
	{
		retVal = true;
		value = val.type;
	}

	if (m == "unit")
	{
		retVal = true;
		value = val.unit;
	}

	if (m == "value")
	{
		retVal = true;
		value = val.value;
	}

	if (m == "group")
	{
		retVal = true;
		value = val.group;
	}

	if (m == "tooltip")
	{
		retVal = true;
		value = val.toolTip;
	}

	if (m == "imagepath")
	{
		retVal = true;
		value = val.imagePath;
	}

	if (m == "help")
	{
		retVal = true;
		value = val.help;
	}

	if (m == "defaultval")
	{
		retVal = true;
		value = val.defaultVal;
	}

	if (m == "displayhint")
	{
		retVal = true;
		value = val.displayHint;
	}

	if (m == "min")
	{
		retVal = true;
		value = val.minStr;
	}

	if (m == "max")
	{
		retVal = true;
		value = val.maxStr;
	}

	if (m == "index1")
	{
		retVal = true;
		value = std::to_string(val.index1);
	}

	if (m == "index2")
	{
		retVal = true;
		value = std::to_string(val.index2);
	}


	if (m == "accesslevel")
	{
		retVal = true;
		value = std::to_string(val.accessLevel);
	}

	if (m == "visible")
	{
		retVal = true;
		if (val.visible)
			value = "1";
		else
			value = "0";
	}

	return retVal;
}

bool SettingsVals::get(type_Vals val, std::string member, double &value)
{
	std::string m = deleteAllSpaces(member);
	boost::to_lower(m);
	bool retVal = false;

	value = 0.0;

	if (m == "min")
	{
		retVal = true;
		type_parse p = parse(val.minStr);
		evaluate(p);

		value = p.result;
	}

	if (m == "max")
	{
		retVal = true;
		type_parse p = parse(val.maxStr);
		evaluate(p);

		value = p.result;
	}

	if (m == "index1")
	{
		retVal = true;
		value = val.index1;
	}

	if (m == "index2")
	{
		retVal = true;
		value = val.index2;
	}

	if (m == "accesslevel")
	{
		retVal = true;
		value = val.accessLevel;
	}

	if (m == "visible")
	{
		retVal = true;

		value = 0.0;
		if (val.visible)
			value = 1.0;
	}

	if (!retVal)
	{
		std::string t;
		retVal = get(val, member, t);

		if (t.size() > 0 && retVal)
		{
			value = std::stod(t);
		}
	}
	return retVal;

}

bool SettingsVals::get(type_Vals val, std::string member, int &value)
{
	std::string m = deleteAllSpaces(member);
	boost::to_lower(m);
	bool retVal = false;

	value = 0;

	if (m == "min")
	{
		retVal = true;
		type_parse p = parse(val.minStr);
		evaluate(p);

		value = (int)round(p.result);
	}

	if (m == "max")
	{
		retVal = true;
		type_parse p = parse(val.maxStr);
		evaluate(p);

		value = (int)round(p.result);
	}

	if (m == "index1")
	{
		retVal = true;
		value = val.index1;
	}

	if (m == "index2")
	{
		retVal = true;
		value = val.index2;
	}

	if (m == "accesslevel")
	{
		retVal = true;
		value = val.accessLevel;
	}

	if (m == "visible")
	{
		retVal = true;

		value = 0;
		if (val.visible)
			value = 1;
	}

	if (!retVal)
	{
		std::string t;
		retVal = get(val, member, t);

		if (t.size()>0 && retVal)
		{
			value = std::stoi(t);
		}
	}
	return retVal;
}

bool SettingsVals::get(type_Vals val, std::string member, bool &value)
{
	std::string m = deleteAllSpaces(member);
	boost::to_lower(m);
	bool retVal = false;

	value = false;

	if (m == "visible")
	{
		retVal = true;
		value = val.visible;
	}

	if (!retVal)
	{
		int x;
		if (get(val, member, x))
		{
			retVal = true;
			value = x > 0;
		}
	}

	return retVal;
}

 bool SettingsVals::getParsedDisplayHint(std::string key, type_parse &retVal)
{
	std::string p;
	type_Vals val;

	if (find(val, key))
	{
		if (get(val, "displayhint", p))
		{
			// parse the string
			retVal = parse(p);
			evaluate(retVal);

			return true;
		}
	}
	return false;

}

 bool SettingsVals::getParsedEnum(std::string key, type_parse &retVal)
 {
	 std::string p;
	 type_Vals val;

	 if (find(val, key))
	 {
		 if (get(val, "type", p))
		 {
			 // parse the string
			 retVal = parse(p);
			 evaluate(retVal);

			 return true;
		 }
	 }
	 return false;
 }

 bool SettingsVals::setValString(std::string key, std::string data)
 {
	 bool rc = false;

	 type_Vals val;

	 if (find(val, key))
	 {
		 val.value = data;
		 rc = true;
	 }
	 bool found = false;

	 std::string comp1 = key;
	 for (int i = 0; i < m_vals.size() && !found; i++)
	 {
		 std::string comp2 = m_vals.at(i).key;
		 //std::transform(comp2.begin(), comp2.end(), comp2.begin(), ::tolower);

		 if (comp2 == comp1)
		 {
			 found = true;
			 m_vals.at(i).value = data;
		 }
	 }

	 return rc;
 }

bool SettingsVals::find(type_Vals &val, std::string key, int index1, int index2, int &position)
{
	std::string comp1 = key;
	std::transform(comp1.begin(), comp1.end(), comp1.begin(), ::tolower);

	bool found = false;
	position = -1;
	for (int i = 0; i < m_vals.size() && !found; i++)
	{
		std::string comp2 = m_vals.at(i).key;
		std::transform(comp2.begin(), comp2.end(), comp2.begin(), ::tolower);

		if ((comp2 == comp1) && ((index1 < 0) || (m_vals.at(i).index1 == index1)) && ((index2 < 0) || (m_vals.at(i).index2 == index2)))
		{
			found = true;
			val = m_vals.at(i);		
			position = i;
		}
	}

	return found;
}

bool SettingsVals::find(type_Vals &val, std::string key, int index1, int index2)
{
	int pos = 0;
	return find(val, key, index1, index2, pos);
}

bool SettingsVals::generateArray(int pos)
{
	type_Vals actVal = m_vals.at(pos);

	// check if there is something to generate
	type_parse p = parse(actVal.displayHint);
	evaluate(p);

	if (p.arraySize1 <= 0) 
		return false;

	int NoOfGeneratedVals=0;

	actVal.arraySize1 = p.arraySize1;
	actVal.arraySize2 = p.arraySize2;
	actVal.arrayID = p.arrayID;

	// generate all indices
	for (int i = 0; i < p.arraySize1; i++)
	{
		if (p.arraySize2 > 0)
		{
			for (int j = 0; j < p.arraySize2; j++)
			{
				actVal.index1 = i;
				actVal.index2 = j;
				actVal.value = actVal.defaultVal;
				m_vals.push_back(actVal);
				NoOfGeneratedVals++;
			}
		}
		else
		{
			actVal.index1 = i;
			actVal.index2 = -1;
			actVal.value = actVal.defaultVal;

			m_vals.push_back(actVal);
			NoOfGeneratedVals++;
		}
	}

	return (NoOfGeneratedVals > 0);
}

void SettingsVals::generateArrays()
{
	std::vector<int> valsToDelete;

	// generate all the arrays
	int last = m_vals.size();
	for (int i = 0; i < last; i++)
		if (generateArray(i))
			valsToDelete.push_back(i);

	// delete identifier for arrays
	for (int i = valsToDelete.size() - 1; i >= 0; i--)
		m_vals.erase(m_vals.begin() + valsToDelete.at(i));
}


bool SettingsVals::readIni(int parseStep, std::string iniFile)
{
	// reads the initialisation file 
	// this file just consists of the key followed by '=' and the value
	// strings are written in ONE LINE!! \n \r and \t are allowed (this one will occure here as \\n \\r and \\t)
	
	std::string fileName = iniFile;

	if (fileName.empty())
		fileName = m_iniFile;

	std::ifstream inFile(fileName, std::ifstream::in);

	if (inFile.is_open())
	{
		// read line by line
		std::string line;
		std::string left;
		std::string right;
		while (!inFile.eof())
		{
			getline(inFile, line);
			// line has format of
			// val=10.0   without index
			// val[1] = 20.0 with one index
			// val[2][3] = 40.0 with two indices

			if (split(line, '=', left, right))
			{
				int index1 = -1;
				int index2 = -1;

				int positionInList;
				type_Vals actVal;
				type_Vals actVal2;

				left = deleteAllChars(left, "\n\r\t ");

				left = getIndex(left, index1, index2);

				if (find(actVal, left, -1, -1, positionInList)) // should be somewhere defined
				{
					// we know: key is existing....
					if (index1 < 0) // just write the value;
					{
						put(actVal, "value", right);
						m_vals.at(positionInList) = actVal;

					}
					else if(parseStep == 2) // check if index is existing
					{
						if (find(actVal2, left, index1, index2, positionInList))
						{
							// just overwrite if index is already existing!
							put(actVal2, "value", right);
							m_vals.at(positionInList) = actVal2;
						}
					}
				}
			}
		}
		inFile.close();
		return true;
	}
	return false;
}

bool SettingsVals::writeIni(std::string iniFile)
{
	// reads the initialisation file 
	// this file just consists of the key followed by '=' and the value
	// strings are written in ONE LINE!! \n \r and \t are allowed (this one will occure here as \\n \\r and \\t)

	std::string fileName = iniFile;

	if (fileName.empty())
		fileName = m_iniFile;

	std::ofstream outFile(fileName, std::ofstream::out);

	if (outFile.is_open())
	{
		// write values line by line
		for (int i = 0; i < m_vals.size(); i++)
		{
			if (m_vals.at(i).index1 < 0)
			{
				outFile << m_vals.at(i).key << "=" << m_vals.at(i).value;
			}
			else
			{
				if (m_vals.at(i).index2 < 0)
				{
					std::string indexStr = std::to_string(m_vals.at(i).index1);
					outFile << m_vals.at(i).key << "[" << indexStr << "]" << "=" << m_vals.at(i).value;
				}
				else
				{
					std::string indexStr1 = std::to_string(m_vals.at(i).index1);
					std::string indexStr2 = std::to_string(m_vals.at(i).index2);
					outFile << m_vals.at(i).key << "[" << indexStr1 << "]" << "[" << indexStr2 << "]" << "=" << m_vals.at(i).value;

				}
			}
			outFile << "\n\r";
		}
		outFile.close();
		return true;
	}
	return false;
}

bool SettingsVals::getValString(std::string key, std::string &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
		return get(found, "value", val);

	return false;

}

bool SettingsVals::getValInt(std::string key, int &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
		return get(found, "value", val);

	return false;

}

bool SettingsVals::getValUInt(std::string key, unsigned int &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
	{
		std::string valString;
		if (get(found, "value", valString))
		{
			val = (unsigned int)std::stoul(valString);
		}

	}
	return false;
}

bool SettingsVals::getValShort(std::string key, short &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
	{
		std::string valString;
		if (get(found, "value", valString))
		{
			val = (short)std::stoul(valString);
		}

	}
	return false;
}

bool SettingsVals::getValUShort(std::string key, unsigned short &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
	{
		std::string valString;
		if (get(found, "value", valString))
		{
			val = (unsigned short)std::stoul(valString);
		}
	}
	return false;
}

bool SettingsVals::getValFloat(std::string key, float &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
	{
		double valDouble;
		if (get(found, "value", valDouble))
		{
			val = (float)valDouble;
			return true;
		}
	}
	return false;
}

bool SettingsVals::getValDouble(std::string key, double &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
		return get(found, "value", val);

	return false;
}

bool SettingsVals::getValBool(std::string key, bool &val, int index1, int index2)
{
	type_Vals found;
	if (find(found, key, index1, index2))
	{
		int valInt;

		if (get(found, "value", valInt))
		{
			val = (valInt > 0);
			return true;
		}
	}
	return false;
}


std::vector<std::string> SettingsVals::getAllGroups()
{
	std::vector<std::string> retVal;
	for (int i = 0; i < m_vals.size(); i++)
	{
		std::string group = m_vals.at(i).group;
		// already in list?
		std::vector<std::string>::iterator it = std::find(retVal.begin(), retVal.end(), group);
		if (it == retVal.end()) // not found!!
			retVal.push_back(group);
	}

	return retVal;
}
