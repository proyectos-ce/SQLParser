#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <regex>
#include "WhereSelector.h"
#include "UpdateDescriptor.h"
#include "ColumnDescriptor.h"

std::string preprocess(std::string sql);

std::string processSelect(std::basic_string<char, std::char_traits<char>, std::allocator<char>> basic_string);

std::string processUpdate(std::string basic_string);

std::vector<WhereSelector> getWheres(std::string wheres, std::string defaultTable);

std::string processCreateTable(std::string basic_string);

std::string processDelete(std::string sql);

std::string processCreateIndex(std::string sql);

std::string processDrop(std::string sql);

char whiteSpaces[] = " \t\r\n";

int main() {
	std::string sql = "  	\nSELECT data, numero,cedula FRoM ESTUDIANTES "
			""
			"\n"
			"\n"
			"WHERE 			ESTUDIANTES.ID > 5 Or ESTUDIANTES.CEDULA = 9 "
			""
			"JOIN CURSOS \n ON ESTUDIANTES.ID = \nCURSOS.IDEST";

	std::string sql2 = "UPDATE table_name\n"
			"SET col1 = value1, column2 = value2\n"
			"WHERE condition < 4 AND COND2 = 5;";

	std::string sqlCreate = "	 CREATE	  TABLE 	 MATRICULA	 (	 IDCURSO STRING 	REFERENCE(	CURSO.ID), IDALUMNO DOUBLE\n"
			"\n"
			"REFERENCE(   ESTUDIANTES.ID    )   ,   NOMBRE    STRING ( 10   ), PRIMARY KEY IDCURSO)";

	std::string sqlIndex = "crEaTe		 IndEX 	  ESTUDIANTES 		";

	std::string sqlDrop = "Delete from	  ESTUDIANTES 		";

	std::string sqlDelete = "DELETE FROM ESTUDIANTES WHERE NOTA > 70";

	std::cerr << preprocess(sqlDrop) << std::endl;

	return 0;
}

bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

std::string preprocess(std::string sql) {
	// Remove newlines and tabs, replace them with spaces
	boost::replace_all(sql, "\r\n", " ");
	boost::replace_all(sql, "\n", " ");
	boost::replace_all(sql, "\t", " ");
	boost::replace_all(sql, ";", "");
	boost::replace_all(sql, " (", "(");
	boost::replace_all(sql, " )", ")");
	boost::replace_all(sql, "( ", "(");
	boost::replace_all(sql, ") ", ")");

	std::string::iterator new_end = std::unique(sql.begin(), sql.end(), BothAreSpaces);
	sql.erase(new_end, sql.end());

	boost::replace_all(sql, " (", "(");
	boost::replace_all(sql, " )", ")");
	boost::replace_all(sql, "( ", "(");
	boost::replace_all(sql, ") ", ")");
	boost::trim(sql);

	std::string::iterator new_end2 = std::unique(sql.begin(), sql.end(), BothAreSpaces);
	sql.erase(new_end2, sql.end());

	if (boost::empty(sql)) {
		std::cerr << "Error: SQL empty" << std::endl;
		throw std::exception();
	}

	std::string firstWord = sql.substr(0, sql.find(" "));
	boost::to_lower(firstWord);
	boost::trim(firstWord);

	std::string presql = sql.substr(sql.find_first_not_of(whiteSpaces, sql.find_first_of(whiteSpaces,
																						 sql.find_first_not_of(
																								 whiteSpaces))));

	if (firstWord == "select") {
		return processSelect(presql);
	} else if (firstWord == "update") {
		return processUpdate(presql);
	} else if (firstWord == "drop") {
		std::string secondWord = presql.substr(0, presql.find(" "));
		boost::to_lower(secondWord);
		boost::trim(secondWord);
		presql = presql.substr(presql.find(" "));
		boost::trim(presql);
		if (secondWord == "table") {
			return processDrop(presql);
		} else {
			std::cerr << "ERROR: Invalid SQL DROP operation " << secondWord << std::endl;
			throw std::exception();
		}
	} else if (firstWord == "delete") {
		return processDelete(presql);
	} else if (firstWord == "create") {
		std::string secondWord = presql.substr(0, presql.find(" "));
		boost::to_lower(secondWord);
		boost::trim(secondWord);
		presql = presql.substr(presql.find(" "));
		boost::trim(presql);
		if (secondWord == "table") {
			return processCreateTable(presql);
		} else if (secondWord == "index") {
			return processCreateIndex(presql);
		} else {
			std::cerr << "ERROR: Invalid SQL CREATE operation " << secondWord << std::endl;
			throw std::exception();
		}
	} else {
		std::cerr << "ERROR: Invalid SQL operation " << firstWord << std::endl;
		throw std::exception();
	}

	return "error";

}

std::string processDrop(std::string sql) {
	std::string table = boost::trim_copy(sql);

	return table;
}

std::string processCreateIndex(std::string sql) {
	std::string table = boost::trim_copy(sql);

	return table;
}


std::string processSelect(std::string sql) {
	std::cout << "VOY A HACER SELECT CON:" << std::endl << sql << std::endl;

	std::regex exp(".+?(?= FROM )", std::regex_constants::icase);


	std::smatch match;
	std::string what;
	std::vector<std::string> whats;
	bool multipleSelect = false;

	// Busca antes del FROM
	if (std::regex_search(sql, match, exp)) {
		what = match.str();
	} else {
		std::cerr << "Error: FROM statement not found" << std::endl;
		throw std::exception();
	}

	// Contiene qué se va a seleccionar
	boost::trim(what);

	// Detecta si hay multiples columnas a seleccionara
	if (boost::contains(what, ",")) {
		std::cout << "MULTIPLES SELECT" << std::endl;

		boost::split(whats, what, boost::is_any_of(","));

		for (auto &element : whats) {
			boost::trim(element);
		}

		multipleSelect = true;

	} else {
		std::cout << "UNICO SELECT" << std::endl;
	}

	boost::regex fromExp("(?<= FROM ).+", boost::regex_constants::icase);
	boost::smatch fromMatches;
	std::string afterFrom;

	if (boost::regex_search(sql, fromMatches, fromExp)) {
		afterFrom = fromMatches.str();
		boost::trim(afterFrom);
	} else {
		std::cerr << "Error: FROM statement invalid" << std::endl;
		throw std::exception();
	}

	size_t spaceLocation = afterFrom.find_first_of(" ");

	std::string from = afterFrom.substr(0, spaceLocation);
	boost::trim(from);

	std::cout << "FROM: " << from << std::endl;

	std::string mainWhere;
	bool multipleWhere = false;
	bool hasWhere = false;
	std::vector<WhereSelector> whereSelectors;
	// Detecta si la consulta tiene un WHERE
	if (boost::contains(boost::to_lower_copy(sql), " where ")) {
		std::cout << "SE DETECTÓ UN WHERE" << std::endl;
		hasWhere = true;

		// Busca entre el WHERE y el JOIN si lo hay (o final de linea)
		boost::regex exp("(?<= WHERE ).*?(?=(?: JOIN |$))", boost::regex_constants::icase);
		boost::smatch match;


		if (boost::regex_search(sql, match, exp)) {
			mainWhere = match.str();
			boost::trim(mainWhere);
		} else {
			std::cerr << "Error: WHERE statement invalid" << std::endl;
			throw std::exception();
		}

		whereSelectors = getWheres(mainWhere, from);

	}

	// Importantes: whereSelectors, whereOperator, multipleWhere, hasWhere

	std::string join;
	std::string joinExternalColumn;
	std::string joinExternalTable;
	std::string joinInternalColumn;
	bool hasJoin = false;

	if (boost::contains(boost::to_lower_copy(sql), " join ")) {
		std::cout << "SE DETECTÓ UN JOIN" << std::endl;
		hasJoin = true;
		size_t found;

		// Busca entre JOIN (texto) WHERE en caso de que hay WHERE o final de linea
		boost::regex exp("(?<= JOIN ).*?(?=(?: WHERE |$))", boost::regex_constants::icase);
		boost::smatch match;

		if (boost::regex_search(sql, match, exp)) {
			join = match.str();
			boost::trim(join);
		} else {
			std::cerr << "Error: JOIN statement invalid" << std::endl;
			throw std::exception();
		}

		found = boost::to_lower_copy(join).find(" on ");

		// Lo obtiene del JOIN [EXTERNAL] ON ....
		joinExternalTable = join.substr(0, found);
		boost::trim(joinExternalTable);

		// Divide lo que le sigue del ON en varias partes para parsearlo por aparte
		std::string matcher = join.substr(found + 4);
		std::vector<std::string> matcherParts;
		boost::split(matcherParts, matcher, boost::is_any_of("="));

		// Parsea por aparte los matches del join (TABLA1.COLUMNA1) = (TABLA2.COLUMNA2)
		bool segundo = false;
		for (auto &matchPart : matcherParts) {
			size_t dotPosition = boost::to_lower_copy(matchPart).find(".");
			if (segundo) {
				std::string checkExternal = matchPart.substr(0, dotPosition);
				boost::trim(checkExternal);

				if (checkExternal != joinExternalTable) {
					std::cerr << "Error: The external table definition on the JOIN clause differs" << std::endl;
					throw std::exception();
				}

				joinExternalColumn = matchPart.substr(dotPosition + 1);
				boost::trim(joinExternalColumn);
			} else {
				std::string checkInternal = matchPart.substr(0, dotPosition);
				boost::trim(checkInternal);

				if (checkInternal != from) {
					std::cerr << "Error: The internal table definition on the JOIN clause differs" << std::endl;
					throw std::exception();
				}

				joinInternalColumn = matchPart.substr(dotPosition + 1);
				boost::trim(joinInternalColumn);
			}


			segundo = true;
		}
		std::cout << "JOIN INTERNAL COLUMN: " << joinInternalColumn << std::endl;
		std::cout << "JOIN EXTERNAL TABLE: " << joinExternalTable << std::endl;
		std::cout << "JOIN EXTERNAL COLUMN: " << joinExternalColumn << std::endl;
	}

	return "error";


}

std::vector<WhereSelector> getWheres(std::string mainWhere, std::string defaultTable) {
	std::vector<std::string> wheres;
	std::string whereOperator = "AND";
	std::vector<WhereSelector> whereSelectors;
	bool multipleWhere = false;

	if (boost::contains(boost::to_lower_copy(mainWhere), "and") ||
		boost::contains(boost::to_lower_copy(mainWhere), " or ")) {
		multipleWhere = true;
		size_t found;

		if (boost::contains(boost::to_lower_copy(mainWhere), " or ")) {
			std::cout << "SE DETECTÓ UN OR" << std::endl;
			found = boost::to_lower_copy(mainWhere).find(" or ");
			whereOperator = "OR";
		} else {
			std::cout << "SE DETECTÓ UN AND" << std::endl;
			found = boost::to_lower_copy(mainWhere).find(" and ");
			whereOperator = "AND";
		}


		wheres.push_back(mainWhere.substr(0, found));
		wheres.push_back(mainWhere.substr(found + 5));
		std::cout << wheres.at(0) << std::endl;
		std::cout << wheres.at(1) << std::endl;

	} else {
		wheres.push_back(mainWhere);
	}

	// En caso de AND, OR, se hace esto por cada Where de forma separada
	for (auto &where : wheres) {

		std::string whereTable;
		std::string whereColumn;
		std::string whereEquals;
		std::string whereOperator = "=";

		// WhereParts contiene (ELEMENTO0) = (ELEMENTO1) del WHERE, donde (ELEMENTO0) = TABLA.COLUMNA
		std::vector<std::string> whereParts;
		boost::split(whereParts, where, boost::is_any_of("=<>"));

		if (boost::contains(where, "<")) {
			whereOperator = "<";
		} else if (boost::contains(where, ">")) {
			whereOperator = ">";
		}

		for (auto &element : whereParts) {
			boost::trim(element);
		}

		if (whereParts.size() != 2) {
			std::cerr << "Error: WHERE statement invalid, related to '=', '<', '>' character" << std::endl;
			throw std::exception();
		}

		whereEquals = whereParts.at(1);

		// WhereParts contiene (ELEMENTO0).(ELEMENTO1) del selector del WHERE, o sea TABLA.COLUMNA
		std::vector<std::string> whereSelectorParts;


		if (boost::contains(whereParts.at(0), ".")) {
			boost::split(whereSelectorParts, whereParts.at(0), boost::is_any_of("."));

			for (auto &element : whereSelectorParts) {
				boost::trim(element);
			}

			if (whereSelectorParts.size() != 2) {
				std::cerr << "Error: WHERE selector invalid" << std::endl;
				throw std::exception();
			}


			whereTable = whereSelectorParts.at(0);
			whereColumn = whereSelectorParts.at(1);
		} else {
			whereColumn = whereParts.at(0);
			whereTable = defaultTable;
		}

		WhereSelector *ws = new WhereSelector();
		ws->column = whereColumn;
		ws->table = whereTable;
		ws->opera = whereOperator;
		ws->equals = whereEquals;
		whereSelectors.push_back(*ws);
	}

	return whereSelectors;
}


std::string processUpdate(std::string sql) {

	// Obtiene la primera palabra que sería la tabla
	std::string table = sql.substr(0, sql.find_first_of(" "));
	std::string setStr;

	// Busca entre SET (texto) WHERE en caso de que hay WHERE o final de linea
	boost::regex exp("(?<= SET ).*?(?=(?: WHERE |$))", boost::regex_constants::icase);
	boost::smatch match;


	if (boost::regex_search(sql, match, exp)) {
		setStr = match.str();
		boost::trim(setStr);
	} else {
		std::cerr << "Error: SET statement invalid" << std::endl;
		throw std::exception();
	}


	std::vector<UpdateDescriptor> sets;

	if (boost::contains(setStr, ",")) {
		std::cout << "MULTIPLES UPDATE" << std::endl;

		std::vector<std::string> setStringVector;
		boost::split(setStringVector, setStr, boost::is_any_of(","));

		for (auto &element : setStringVector) {
			boost::trim(element);

			size_t equalPosition = element.find("=");

			UpdateDescriptor *ud = new UpdateDescriptor();
			ud->column = boost::trim_copy(element.substr(0, equalPosition));
			ud->value = boost::trim_copy(element.substr(equalPosition));
			sets.push_back(*ud);

		}

	} else {
		std::cout << "UNICO SELECT" << std::endl;

		size_t equalPosition = setStr.find("=");

		UpdateDescriptor *ud = new UpdateDescriptor();
		ud->column = boost::trim_copy(setStr.substr(0, equalPosition));
		ud->value = boost::trim_copy(setStr.substr(equalPosition));
		sets.push_back(*ud);
	}


	std::vector<WhereSelector> wheres;

	if (boost::contains(boost::to_lower_copy(sql), " where ")) {
		std::cout << "SE DETECTÓ UN WHERE" << std::endl;
		std::string mainWhere;

		// Busca entre el WHERE y el JOIN si lo hay (o final de linea)
		boost::regex exp("(?<= WHERE ).*?(?=(?: SET |$))", boost::regex_constants::icase);
		boost::smatch match;

		std::cout << sql << std::endl;


		if (boost::regex_search(sql, match, exp)) {
			mainWhere = match.str();
			boost::trim(mainWhere);
		} else {
			std::cerr << "Error: WHERE statement invalid" << std::endl;
			throw std::exception();
		}

		wheres = getWheres(mainWhere, table);

	}

	std::cout << wheres.at(1).table << std::endl;

	return "error";

}


std::string processCreateTable(std::string sql) {
	std::cout << "VOY A CREAR UNA TABLA" << std::endl;

	std::string tableName = sql.substr(0, sql.find_first_of("("));
	boost::trim(tableName);

	std::string primaryKey;

	std::vector<ColumnDescriptor> columns;

	std::cout << "TABLE NAME: " << tableName << std::endl;


	if (sql.substr(sql.size() - 1) != ")") {
		std::cerr << "Error: Invalid query (no ending parenthesis)" << std::endl;
		throw std::exception();
	}

	sql = sql.substr(sql.find_first_of("(") + 1);
	sql = sql.substr(0, sql.size() - 1);

	std::cout << sql << std::endl;

	std::vector<std::string> columnsStr;

	boost::split(columnsStr, sql, boost::is_any_of(","));

	for (auto &column : columnsStr) {
		std::vector<std::string> words;
		boost::trim(column);
		ColumnDescriptor* cd = new ColumnDescriptor();


		boost::split(words, column, boost::is_any_of("\t "), boost::token_compress_on);

		if (boost::to_lower_copy(words.at(0)) == "primary" && words.size() == 3 && boost::to_lower_copy(words.at(1)) == "key") {
				primaryKey = words.at(2);
				boost::trim(primaryKey);
			continue;
		} else if (words.size() == 3 && boost::to_lower_copy(words.at(2).substr(0, 10)) == "reference(" && words.at(2).substr(words.at(2).size() - 1) == ")") {
			std::cout << "SE DETECTÓ UNA REFERENCIA" << std::endl;
			std::string refTemp = words.at(2).substr(10);
			boost::trim(refTemp);
			refTemp = refTemp.substr(0,refTemp.size() - 1);

			size_t dotPosition = refTemp.find(".");

			cd->referenceTable = refTemp.substr(0, dotPosition);
			cd->referenceColumn = refTemp.substr(dotPosition + 1);
			cd->reference = true;

			boost::trim(cd->referenceColumn);
			boost::trim(cd->referenceTable);

			std::cout << refTemp << std::endl;

		} else if (words.size() != 2) {
			std::cerr << "Error: The column definition is invalid" << std::endl;
			throw std::exception();
		}


		cd->name = words.at(0);

		std::string type = boost::to_lower_copy(words.at(1));
		boost::trim(type);

		if (type == "string") {
			cd->type = 0;
		} else if (type == "int") {
			cd->type = 1;
		} else if (type == "double") {
			cd->type = 2;
		} else {
			std::cerr << "Error: Invalid column type" << std::endl;
			throw std::exception();
		}



		columns.push_back(*cd);

	}

	std::cout <<  "PRIMARY KEY:" << primaryKey << std::endl;

	std::string columnTypes;
	bool notFirst = false;


	for (auto& column : columns) {
		if (notFirst) {
			columnTypes += ",";
		}
		columnTypes += std::to_string(column.type);
		notFirst = true;
	}

	std::string columnNames;
	notFirst = false;

	for (auto& column : columns) {
		if (notFirst) {
			columnNames += ",";
		}
		columnNames += '"' + column.name + '"';
		notFirst = true;
	}

	std::string references;
	notFirst = false;

	for (auto& column : columns) {
		if (column.reference) {
			if (notFirst) {
				references += ",";
			}
			references += "{\"column\": \"";
			references += column.name;
			references += "\", \"extTable\": \"";
			references += column.referenceTable;
			references += "\", \"extColumn\": \"";
			references += column.referenceColumn;
			references += "\"}";
			notFirst = true;
		}
	}


	return "{  \n"
			"   \"command\":\"create_table\",\n"
			"   \"name\":\"" + tableName + "\",\n"
			"   \"columnTypes\":[" + columnTypes + "],\n"
			"   \"columnNames\":[" + columnNames + "],\n"
			"   \"references\":[" + references + "],\n"
			"   \"primaryKey\":\"" + primaryKey + "\",\n"
			"   \"rows\":[]\n"
			"}";


}


std::string processDelete(std::string sql) {
	std::cout << "VOY A HACER UN DELETE" << std::endl;


	boost::regex fromExp("(?<=FROM ).+", boost::regex_constants::icase);
	boost::smatch fromMatches;
	std::string afterFrom;


	if (boost::regex_search(sql, fromMatches, fromExp)) {
		afterFrom = fromMatches.str();
		boost::trim(afterFrom);
	} else {
		std::cerr << "Error: FROM statement invalid" << std::endl;
		throw std::exception();
	}

	size_t spaceLocation = afterFrom.find_first_of(" ");

	std::string from = afterFrom.substr(0, spaceLocation);
	boost::trim(from);

	std::cout << "FROM: " << from << std::endl;


	std::string mainWhere;
	bool multipleWhere = false;
	bool hasWhere = false;
	std::vector<WhereSelector> whereSelectors;
	// Detecta si la consulta tiene un WHERE
	if (boost::contains(boost::to_lower_copy(sql), " where ")) {
		std::cout << "SE DETECTÓ UN WHERE" << std::endl;
		hasWhere = true;

		// Busca entre el WHERE y el JOIN si lo hay (o final de linea)
		boost::regex exp("(?<= WHERE ).*?(?=(?: JOIN |$))", boost::regex_constants::icase);
		boost::smatch match;


		if (boost::regex_search(sql, match, exp)) {
			mainWhere = match.str();
			boost::trim(mainWhere);
		} else {
			std::cerr << "Error: WHERE statement invalid" << std::endl;
			throw std::exception();
		}

		whereSelectors = getWheres(mainWhere, from);

	}


	return "error";
}