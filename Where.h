//
// Created by joseph on 17/06/17.
//

#ifndef PARSER_WHERE_H
#define PARSER_WHERE_H

#include <vector>
#include "WhereSelector.h"

class Where {
public:
	std::vector<WhereSelector> selectors;
	std::string opera;
	Where() {

	}
};

#endif //PARSER_WHERE_H
