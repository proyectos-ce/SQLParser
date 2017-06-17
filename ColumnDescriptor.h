//
// Created by joseph on 17/06/17.
//

#ifndef PARSER_TABLEDESCRIPTOR_H
#define PARSER_TABLEDESCRIPTOR_H


class ColumnDescriptor {
public:
	std::string name;
	int type;

	bool reference = false;
	std::string referenceTable;
	std::string referenceColumn;

	ColumnDescriptor() {

	}
};


#endif //PARSER_TABLEDESCRIPTOR_H
