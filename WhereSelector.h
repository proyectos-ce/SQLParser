//
// Created by joseph on 16/06/17.
//

#ifndef PARSER_WHERESELECTOR_H
#define PARSER_WHERESELECTOR_H

#include <iostream>

class WhereSelector {

public:
	std::string column;
	std::string table;
	std::string opera;
	std::string equals;

	WhereSelector() {

	}
};


#endif //PARSER_WHERESELECTOR_H
