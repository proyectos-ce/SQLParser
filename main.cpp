#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <regex>
#include "string"

void preprocess(std::string sql);

void processSelect(std::basic_string<char, std::char_traits<char>, std::allocator<char>> basic_string);

char whiteSpaces[] = " \t\r\n";

int main() {
	std::string sql = "SELECT data, numero,cedula FRoM ESTUDIANTES "
			""
			"\n"
			"\n"
			"WHERE 			ESTUDIANTES.ID = 5";

	preprocess(sql);

	return 0;
}

void preprocess(std::string sql) {
	// Remove newlines and tabs, replace them with spaces
	boost::replace_all(sql, "\r\n", " ");
	boost::replace_all(sql, "\n", "");
	boost::replace_all(sql, "\t", " ");
	boost::trim(sql);

	if (boost::empty(sql)) {
		std::cerr << "Error: SQL empty" << std::endl;
		throw std::exception();
	}

	std::string firstWord = sql.substr(0, sql.find(" "));
	boost::to_lower(firstWord);
	boost::trim(firstWord);

	if (firstWord == "select") {
		processSelect(sql.substr(sql.find_first_not_of(whiteSpaces, sql.find_first_of(whiteSpaces,
																					  sql.find_first_not_of(
																							  whiteSpaces)))));
	}

}

void processSelect(std::string sql) {
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


	std::string where;
	std::string whereTable;
	std::string whereColumn;
	std::string whereEquals;
	bool hasWhere = false;

	// Detecta si la consulta tiene un WHERE
	if (boost::contains(boost::to_lower_copy(sql), " where ")) {
		std::cout << "SE DETECTÓ UN WHERE" << std::endl;
		hasWhere = true;

		// Busca entre el WHERE y el JOIN si lo hay (o final de linea)
		boost::regex exp("(?<= WHERE ).*?(?=(?: JOIN |$))", boost::regex_constants::icase);
		boost::smatch match;


		if (boost::regex_search(sql, match, exp)) {
			where = match.str();
			boost::trim(where);
		} else {
			std::cerr << "Error: WHERE statement invalid" << std::endl;
			throw std::exception();
		}

		// WhereParts contiene (ELEMENTO0) = (ELEMENTO1) del WHERE, donde (ELEMENTO0) = TABLA.COLUMNA
		std::vector<std::string> whereParts;
		boost::split(whereParts, where, boost::is_any_of("="));

		for (auto &element : whereParts) {
			boost::trim(element);
		}

		if (whereParts.size() != 2) {
			std::cerr << "Error: WHERE statement invalid, related to '=' character" << std::endl;
			throw std::exception();
		}

		whereEquals = whereParts.at(1);

		// WhereParts contiene (ELEMENTO0).(ELEMENTO1) del selector del WHERE, o sea TABLA.COLUMNA
		std::vector<std::string> whereSelectorParts;

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

		std::cout << whereEquals << std::endl;

	}

	std::string join;
	std::string joinExternalColumn;
	std::string joinExternalTable;
	std::string joinInternalColumn;
	bool hasJoin = false;

	if (boost::contains(boost::to_lower_copy(sql), " join ")) {
		std::cout << "SE DETECTÓ UN JOIN" << std::endl;
		hasJoin = true;
	}


}