//
// Created by Zhao Xiaodong on 2017/6/4.
//

#include <iostream>
#include <string>
#include <strstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <regex>
#include "RecordManager.h"
#include "catalogmanager.h"
#include "buffermanager.h"
#include "table.h"

#define DEBUG_IT false

using namespace std;

extern block blocks[BLOCKNUMBER];

bool RecordManager::insertRecord(string rawValues) {
	tableInfo = catalogMgr.GetTable(tableName);

	const bool NO_SUCH_TABLE_ERROR = false; // for test
	if (NO_SUCH_TABLE_ERROR) {
		// TODO: 没有该表如何
		cerr << "Error: " << "No such table named: " << tableName << endl;
	}

	vector<IndexInfo> indexInfos;

	string resultRecord = generateInsertValues(rawValues, indexInfos); // rawData used for get index info
#if DEBUG_IT
	cout << "Insert record:" << endl;
	cout << resultRecord;
	cout << "TableName: " << tableName << endl;
#endif

	buffermanager bufferMgr;

	int blockNum;
	int blockOffset;

	bufferMgr.FindSuitBlockinBuffer(tableName + ".rec", resultRecord.length(), &blockNum, &blockOffset);
#if DEBUG_IT
	cout << "blockNum: " << blockNum << " offset: " << blockOffset << endl;
#endif
	if (resultRecord != "") {
		bufferMgr.Insert(blockNum, blockOffset, (char *)resultRecord.c_str()); // TODO: 返回值？成功失败的标志？
	}

#if DEBUG_IT
#endif

	if (resultRecord != "") {
		for (IndexInfo indexInfo : indexInfos) {
			indexInfo.value;
			string indexFile = indexInfo.indexName + ".idx";
			bPlusTree.Insert(indexInfo.type, &indexFile, &indexInfo.value, blockNum, blockOffset);
		}
	}

	return true;
}

string RecordManager::generateInsertValues(string rawValues, vector<IndexInfo> &indexInfos) {
	string resultRecord;
	int tmp_i;
	float tmp_f;
	string tmp_s;

	// TODO: not check space in string
	rawValues.erase(remove_if(rawValues.begin(), rawValues.end(), [](char x) { return isspace(x); }), rawValues.end());
	istrstream strIn(rawValues.c_str(), rawValues.length());

	for (auto iter = tableInfo.Attr.begin(); iter != tableInfo.Attr.end(); iter++) {
		if (iter != tableInfo.Attr.begin() && iter != tableInfo.Attr.end())
			resultRecord += " ";

		string valueTerm;
		string currentValueStr;

		getline(strIn, valueTerm, ','); // TODO: not check comma in string
		istrstream strTermIn(valueTerm.c_str(), valueTerm.length());
		switch ((*iter).type) {
		case int_t:
			strTermIn >> tmp_i; // TODO: check typeError
			currentValueStr = to_string(tmp_i);
			resultRecord += currentValueStr;
			break;
		case myFloat:
			strTermIn >> tmp_f;
			currentValueStr = to_string(tmp_f);
			resultRecord += currentValueStr;
			break;
		case char_t:
			strTermIn >> tmp_s;
			if (*tmp_s.begin() != '\'' || *(tmp_s.end() - 1) != '\'') {
				cerr << "Error: " << "string input syntax error!\n" << valueTerm << endl;
				return "";
			}
			tmp_s.erase(tmp_s.begin());
			tmp_s.erase(tmp_s.end() - 1);
			currentValueStr = tmp_s;
			resultRecord += currentValueStr;
			break;
		default:
			break;
		}
		if (strTermIn.bad()) {
			cerr << "Error: " << "value not match error!" << endl;
			return "";
		}
		if ((strTermIn >> tmp_s)) {
			cerr << "Error: " << "value type syntax error!" << endl;
#if DEBUG_IT
			cout << tmp_s << endl;
#endif
			return "";
		}

		if ((*iter).unique || (*iter).primary) {
			//            checkUnique() TODO: check unique: after done select
			if (!checkUnique(*iter, currentValueStr)) {
				cerr << "Error: " << "unique value duplicate!" << endl;
				return "";
			}
		}

		if ((*iter).indexname != "noindex") {
			IndexInfo indexInfo;
			indexInfo.type = static_cast<DataType>((*iter).type);
			indexInfo.value = valueTerm;
			indexInfo.indexName = (*iter).indexname;
			indexInfos.push_back(indexInfo);
		}
	}
	if (strIn.bad()) {
		cerr << "Error: " << "values list error!" << endl;
		return "";
	}
	if ((strIn >> tmp_s)) {
		cerr << "Error: " << "values list not match error!" << endl;
#if DEBUG_IT
		cout << tmp_s << endl;
#endif
		return "";
	}
	resultRecord += "\n";

	return resultRecord;
}

bool RecordManager::selectRecords(vector<string> &attributes, string rawWhereClause) {
	tableInfo = catalogMgr.GetTable(tableName); // TODO: no such table???

	if (rawWhereClause.empty()) {
		int blockNum = bufferMgr.FindBlockinBuffer(tableName + ".rec", 0);
#if DEBUG_IT
		cout << blocks[blockNum].content;
#endif
		char *pos = blocks[blockNum].content;
		istrstream tuplesStrIn(pos);
		string tuple;
		while (getline(tuplesStrIn, tuple)) {
			if (tuple[0] == ' ') {
				continue;
			}
			cout << projectTuple(tuple, attributes);
			cout << endl;
		}
	}
	else {
		vector<Restrict *> restricts = parseWhere(rawWhereClause); // TODO: 记得delete掉
		vector<Range *> ranges = generateRange(restricts);

		for (Range *range : ranges) {
#if DEBUG_IT
			cout << "Range output: ";
			cout << range->type << " " << range->attrName << " " << range->valid << endl;
#endif
			vector<Attribute>::iterator attrIter = find_if(tableInfo.Attr.begin(), tableInfo.Attr.end(), [range](Attribute attr) {
				return (attr.attrname == range->attrName && attr.indexname != "noindex");
			});
			if (attrIter != tableInfo.Attr.end()) {
				// find in index
				string indexFile = (*attrIter).indexname + ".idx";
				switch (range->type) {
				case int_t: {
					IntRange *intRange = static_cast<IntRange *>(range);
					// TODO: parameter adjustment, and min, max include specify
					string minValStr = to_string(intRange->minValue);
					string maxValStr = to_string(intRange->maxValue);
#if 0
					cout << "Range: " << intRange->minValue << intRange->maxValue << endl;
#endif
					bPlusTree.Find(range->type, &indexFile, &minValStr, &maxValStr, !(intRange->includeMin), !(intRange->includeMax));
					break;
				}
				case myFloat: {
					FloatRange *floatRange = static_cast<FloatRange *>(range);
					string minValStr = to_string(floatRange->minValue);
					string maxValStr = to_string(floatRange->maxValue);
					bPlusTree.Find(range->type, &indexFile, &minValStr, &maxValStr, !(floatRange->includeMin), !(floatRange->includeMax));
					break;
				}
				case char_t: {
					StringRange *stringRange = static_cast<StringRange *>(range);
					// TODO: string 该如何定
					if (stringRange->value != "")
						bPlusTree.Find(range->type, &indexFile, &stringRange->value, &stringRange->value, false, false);
					break;
				}
				default:
					break; // only check one index
				}
				checkSelectTuple(ranges, attributes);
				break;
			}
			else {
				// TODO: no index
				checkSelectTupleInBuffer(ranges, attributes);
				break;
			}
		}
	}

	return true;
}

bool RecordManager::deleteRecords(string rawWhereClause = "") {
	//    catalogmanager catalogMgr;
	tableInfo = catalogMgr.GetTable(tableName);

	if (rawWhereClause.empty()) {
		// TODO: simple fetch all
//        buffermanager bufferMgr;
		int blockNum = bufferMgr.FindBlockinBuffer(tableName + ".rec", 0); // TODO: fetch all block, connect with catalog
#if DEBUG_IT
		cout << blocks[blockNum].content;
#endif
		char *pos = blocks[blockNum].content;

		// use fill blank to delete, consider extract it into a standalone function
		while ((*pos) != '\0') {
			while ((*pos) != '\n' && (*pos) != '\0') {
				(*pos) = ' ';
				pos++;
			}
			pos++;
		}

#if DEBUG_IT
		cout << "After Delete:" << endl;
		cout << blocks[blockNum].content;
#endif
	}
	else {
		vector<Restrict *> restricts = parseWhere(rawWhereClause); // TODO: 记得delete掉
		vector<Range *> ranges = generateRange(restricts);
#if 0
		for (Range *range : ranges) {
			cout << "Range output: ";
			cout << range->type << " " << range->attrName << " " << range->valid << endl;
		}

#endif
#if 0
		for (Attribute attr : tableInfo.Attr) {
			cout << attr.attrname << endl;
		}
#endif
		for (Range *range : ranges) {
#if DEBUG_IT
			cout << "Range output: ";
			cout << range->type << " " << range->attrName << " " << range->valid << endl;
#endif
			vector<Attribute>::iterator attrIter = find_if(tableInfo.Attr.begin(), tableInfo.Attr.end(), [range](Attribute attr) {
				return (attr.attrname == range->attrName && attr.indexname != "noindex");
			});
			if (attrIter != tableInfo.Attr.end()) {
				// find in index
				string indexFile = (*attrIter).indexname + ".idx";
				switch (range->type) {
				case int_t: {
					IntRange *intRange = static_cast<IntRange *>(range);
					// TODO: parameter adjustment, and min, max include specify
					string minValStr = to_string(intRange->minValue);
					string maxValStr = to_string(intRange->maxValue);
#if 0
					cout << "Range: " << intRange->minValue << intRange->maxValue << endl;
#endif
					bPlusTree.Find(range->type, &indexFile, &minValStr, &maxValStr, !(intRange->includeMin), !(intRange->includeMax)); // TODO: file name 到底是啥？
					break;
				}
				case myFloat: {
					FloatRange *floatRange = static_cast<FloatRange *>(range);
					string minValStr = to_string(floatRange->minValue);
					string maxValStr = to_string(floatRange->maxValue);
					bPlusTree.Find(range->type, &indexFile, &minValStr, &maxValStr, !(floatRange->includeMin), !(floatRange->includeMax)); // TODO: file name 到底是啥？
					break;
				}
				case char_t: {
					StringRange *stringRange = static_cast<StringRange *>(range);
					// TODO: string 该如何定
					if (stringRange->value != "")
						bPlusTree.Find(range->type, &indexFile, &stringRange->value, &stringRange->value, false, false);
					break;
				}
				default:
					break; // only check one index
				}
				checkDeleteTuple(ranges);
				break;
			}
			else {
				// TODO: no index
				checkDeleteTupleInBuffer(ranges);
				break;
			}
		}
	}
	return true;
}

vector<Restrict *> RecordManager::parseWhere(string rawWhereClause) {
	vector<Restrict *> restricts;
	regex andRegex("\\s+and\\s+");
	sregex_token_iterator
		first{ rawWhereClause.begin(), rawWhereClause.end(), andRegex, -1 },
		last;
	vector<string> restrictions{ first, last };

	for (string restriction : restrictions) {
		RelationOp relationOp = invalidOp;
		for (int i = (int)ne; i != (int)eq + 1; i++) {
			RelationOp op = static_cast<RelationOp>(i);
			if (restriction.find(relationOps.find(op)->second) != string::npos) {
				relationOp = op;
				break;
			}
		}
		if (relationOp == invalidOp) {
			cerr << "Error: " << "yet unsupported operation." << endl;
			return vector<Restrict *>(0);
		}
		string attrName = restriction.substr(0, restriction.find(relationOps.find(relationOp)->second));
		attrName.erase(remove_if(attrName.begin(), attrName.end(), [](char x) { return isspace(x); }), attrName.end());

		string value = restriction.substr(restriction.find(relationOps.find(relationOp)->second), string::npos);
		value.erase(value.begin(), value.begin() + (relationOps.find(relationOp)->second).length());
		value.erase(remove_if(value.begin(), value.end(), [](char x) { return isspace(x); }), value.end());

#if DEBUG_IT
		cout << "[" << attrName << "]" << relationOps.find(relationOp)->second << "[" << value << "]" << endl;
#endif

#if 1
		vector<Attribute>::iterator attrIter;
#if 0
		cout << "attr num: ";
		cout << tableInfo.Attr.size();
		cout << endl;
#endif
		for (attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
			if (attrIter->attrname == attrName) {
				istrstream valueStrIn(value.c_str(), value.length());

				string tmp;
				switch (attrIter->type) {
				case int_t: {
					int tmp_i;
					if (valueStrIn >> tmp_i && !(valueStrIn >> tmp)) {
						Restrict *intRestrict = new IntRestrict(attrName, relationOp, tmp_i);
						restricts.push_back(intRestrict);
					}
					else {
						cerr << "Error: " << " type not match!" << endl;
					}
					break;
				}
				case myFloat: {
					float tmp_f;
					if (valueStrIn >> tmp_f && !(valueStrIn >> tmp)) {
						Restrict *floatRestrict = new FloatRestrict(attrName, relationOp, tmp_f);
						restricts.push_back(floatRestrict);
					}
					else {
						cerr << "Error: " << " type not match!" << endl;
					}
					break;

				}
				case char_t: {
					string tmp_s;
					if (valueStrIn >> tmp_s) {
						if (*tmp_s.begin() == '\'' && *(tmp_s.end() - 1) == '\'') {
							Restrict *stringRestrict =
								new StringRestrict(attrName, relationOp, tmp_s.substr(1, tmp_s.length() - 2));
							restricts.push_back(stringRestrict);
						}
					}
					else {
						cerr << "Error: " << " type not match!" << endl;
					}
					break;
				}
				default:
					break;
				}
				break;
			}
		}
		if (attrIter == tableInfo.Attr.end()) {
			cerr << "Error: " << "No such attribute" << endl;
			return vector<Restrict *>(0);
		}
#endif
	}

#if DEBUG_IT
	cout << "Restricts: " << endl;
	for (Restrict *restrict : restricts) {
		cout << restrict->attrName << relationOps.find(restrict->op)->second;
		switch (restrict->type) {
		case int_t: {
			cout << ((IntRestrict *)(restrict))->value << endl;
			break;
		}
		case myFloat: {
			cout << ((FloatRestrict *)(restrict))->value << endl;
			break;
		}
		case char_t: {
			cout << ((StringRestrict *)(restrict))->value << endl;
			break;
		}
		default:
			break;
		}
	}
#endif
	return restricts;
}

vector<Range *> RecordManager::generateRange(const vector<Restrict *> &restricts) {
	vector<Range *> ranges;

	// TODO: problem with check restrict
	for (Restrict *restrict : restricts) {
		string attrName = restrict->attrName;
		vector<Range *>::iterator rangeIter = find_if(ranges.begin(), ranges.end(), [attrName](Range *range) -> bool {
			return range->attrName == attrName;
		});

		if (rangeIter == ranges.end()) {
			switch (restrict->type) {
			case int_t: {
				IntRange *intRange = new IntRange(attrName);
				IntRestrict *intRestrict = static_cast<IntRestrict *>(restrict);
				switch (intRestrict->op) {
				case ne:
					intRange->excludeValues.push_back(intRestrict->value);
					break;
				case nlt:
					intRange->minValue = intRestrict->value;
					intRange->includeMin = true;
					break;
				case ngt:
					intRange->maxValue = intRestrict->value;
					intRange->includeMax = true;
					break;
				case lt:
					intRange->maxValue = intRestrict->value;
					intRange->includeMax = false;
					break;
				case gt:
					intRange->minValue = intRestrict->value;
					intRange->includeMin = false;
					break;
				case eq:
					intRange->minValue = intRange->maxValue = intRestrict->value;
					intRange->includeMax = intRange->includeMin = true;
					break;
				default:
					break;
				}
				ranges.push_back(static_cast<Range *>(intRange));
				break;
			}
			case myFloat: {
				FloatRange *floatRange = new FloatRange(attrName);
				FloatRestrict *floatRestrict = static_cast<FloatRestrict *>(restrict);
				switch (floatRestrict->op) {
				case ne:
					floatRange->excludeValues.push_back(floatRestrict->value);
					break;
				case nlt:
					floatRange->minValue = floatRestrict->value;
					floatRange->includeMin = true;
					break;
				case ngt:
					floatRange->maxValue = floatRestrict->value;
					floatRange->includeMax = true;
					break;
				case lt:
					floatRange->maxValue = floatRestrict->value;
					floatRange->includeMax = false;
					break;
				case gt:
					floatRange->minValue = floatRestrict->value;
					floatRange->includeMin = false;
					break;
				case eq:
					floatRange->minValue = floatRange->maxValue = floatRestrict->value;
					floatRange->includeMax = floatRange->includeMin = true;
					break;
				default:
					break;
				}
				ranges.push_back(static_cast<Range *>(floatRange));
				break;
			}
			case char_t: {
				StringRange *stringRange = new StringRange(attrName);
				StringRestrict *stringRestrict = static_cast<StringRestrict *>(restrict);
				switch (stringRestrict->op) {
				case ne:
					stringRange->excludeValues.push_back(stringRestrict->value);
					break;
				case eq:
					stringRange->value = stringRestrict->value;
				default:
					break;
				}
				ranges.push_back(static_cast<Range *>(stringRange));
				break;
			}
			}
		}
		else {
			if (!updateRange(*rangeIter, restrict)) {
				ranges.clear();
				return ranges;
			}
		}
	}
	return ranges;
}

bool RecordManager::updateRange(Range *range, Restrict *restrict) {
	switch (restrict->type) {
	case int_t: {
		IntRestrict *intRestrict = static_cast<IntRestrict *>(restrict);
		IntRange *intRange = static_cast<IntRange *>(range);
		switch (restrict->op) {
		case ne: {
			intRange->excludeValues.push_back(intRestrict->value);
			break;
		}
		case nlt: {
			if (intRestrict->value >= intRange->maxValue && !intRange->includeMax ||
				intRestrict->value > intRange->maxValue) {
				return false;
			}
			if (intRestrict->value > intRange->minValue) {
				intRange->minValue = intRestrict->value;
				intRange->includeMin = true;
			}
			break;
		}
		case ngt: {
			if (intRestrict->value <= intRange->minValue && !intRange->includeMin ||
				intRestrict->value < intRange->minValue) {
				return false;
			}
			if (intRestrict->value < intRange->maxValue) {
				intRange->maxValue = intRestrict->value;
				intRange->includeMax = true;
			}
			break;
		}
		case lt: {
			if (intRestrict->value < intRange->minValue ||
				intRestrict->value == intRange->minValue && !intRange->includeMin) {
				return false;
			}
			if (intRestrict->value < intRange->maxValue ||
				intRestrict->value == intRange->maxValue && intRange->includeMax) {
				intRange->maxValue = intRestrict->value;
				intRange->includeMax = false;
			}
			break;
		}
		case gt: {
			if (intRestrict->value > intRange->maxValue ||
				intRestrict->value == intRange->maxValue && !intRange->includeMax) {
				return false;
			}
			if (intRestrict->value > intRange->minValue ||
				intRestrict->value == intRange->minValue && intRange->includeMin) {
				intRange->minValue = intRestrict->value;
				intRange->includeMin = false;
			}
			break;
		}
		case eq:
			if (intRestrict->value > intRange->maxValue ||
				intRestrict->value < intRange->minValue ||
				intRestrict->value == intRange->maxValue && !intRange->includeMax ||
				intRestrict->value == intRange->minValue && !intRange->includeMin) {
				return false;
			}
			if (intRange->excludeValues.end() !=
				find(intRange->excludeValues.begin(), intRange->excludeValues.end(), intRestrict->value)) {
				return false;
			}
			break;
		default:
			break;
		}
		break;
	}
	case myFloat: { // TODO: need to refactor
		FloatRestrict *floatRestrict = static_cast<FloatRestrict *>(restrict);
		FloatRange *floatRange = static_cast<FloatRange *>(range);
		switch (restrict->op) {
		case ne: {
			floatRange->excludeValues.push_back(floatRestrict->value);
			break;
		}
		case nlt: {
			if (floatRestrict->value >= floatRange->maxValue && !floatRange->includeMax ||
				floatRestrict->value > floatRange->maxValue) {
				return false;
			}
			if (floatRestrict->value > floatRange->minValue) {
				floatRange->minValue = floatRestrict->value;
				floatRange->includeMin = true;
			}
			break;
		}
		case ngt: {
			if (floatRestrict->value <= floatRange->minValue && !floatRange->includeMin ||
				floatRestrict->value < floatRange->minValue) {
				return false;
			}
			if (floatRestrict->value < floatRange->maxValue) {
				floatRange->maxValue = floatRestrict->value;
				floatRange->includeMax = true;
			}
			break;
		}
		case lt: {
			if (floatRestrict->value < floatRange->minValue ||
				floatRestrict->value == floatRange->minValue && !floatRange->includeMin) {
				return false;
			}
			if (floatRestrict->value < floatRange->maxValue ||
				floatRestrict->value == floatRange->maxValue && floatRange->includeMax) {
				floatRange->maxValue = floatRestrict->value;
				floatRange->includeMax = false;
			}
			break;
		}
		case gt: {
			if (floatRestrict->value > floatRange->maxValue ||
				floatRestrict->value == floatRange->maxValue && !floatRange->includeMax) {
				return false;
			}
			if (floatRestrict->value > floatRange->minValue ||
				floatRestrict->value == floatRange->minValue && floatRange->includeMin) {
				floatRange->minValue = floatRestrict->value;
				floatRange->includeMin = false;
			}
			break;
		}
		case eq:
			if (floatRestrict->value > floatRange->maxValue ||
				floatRestrict->value < floatRange->minValue ||
				floatRestrict->value == floatRange->maxValue && !floatRange->includeMax ||
				floatRestrict->value == floatRange->minValue && !floatRange->includeMin) {
				return false;
			}
			if (floatRange->excludeValues.end() !=
				find(floatRange->excludeValues.begin(), floatRange->excludeValues.end(),
					floatRestrict->value)) {
				return false;
			}
			break;
		default:
			break;
		}
		break;
	}
	case char_t: {
		StringRestrict *stringRestrict = static_cast<StringRestrict *>(restrict);
		StringRange *stringRange = static_cast<StringRange *>(range);
		switch (restrict->op) {
		case ne: {
			if (stringRange->value == stringRestrict->value) {
				return false;
			}
			if (stringRange->value == "") {
				stringRange->excludeValues.push_back(stringRestrict->value);
			}
			break;
		}
		case eq: {
			if (stringRange->value != "" && stringRange->value != stringRestrict->value) {
				return false;
			}
			if (stringRange->excludeValues.end() !=
				find(stringRange->excludeValues.begin(), stringRange->excludeValues.end(),
					stringRestrict->value)) {
				return false;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	return true;
}

void RecordManager::checkDeleteTuple(vector<Range *> &ranges) {
	const string file = "select.tmp";
	std::ifstream posFileStream(file);
	if (!posFileStream.is_open()) {
		std::cout << "failed to open " << file << '\n';
	}
	else {
		int blockNumInFile, blockOffsetInFile;

		map<string, KeysWithType> updatedKeys; // to be updated according to delete

		for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
			if (attrIter->indexname != "noindex") {
				KeysWithType keys;
				updatedKeys.insert(pair<string, KeysWithType>(attrIter->indexname, keys));
			}
		}

		while (posFileStream >> blockNumInFile >> blockOffsetInFile) {
#if DEBUG_IT
			cout << blockNumInFile << " " << blockOffsetInFile << endl;
#endif
			int blockNum;
			blockNum = bufferMgr.FindBlockinBuffer(tableInfo.getName() + ".rec", blockNumInFile);
			char *tuplePos = bufferMgr.GetDetail(blockNum, blockOffsetInFile);
			char *newLineChar = strchr(tuplePos, '\n');
			int size = newLineChar - tuplePos;
			istrstream tupleStrIn(tuplePos, size);
#if 0
			cout << "Tuple size: " << size << endl;
#endif

#if DEBUG_IT
			//            cout << "Tuple: " << tuplePos << endl;
			//            cout << "Tuple stream: " << tupleStrIn.str();
#endif
			map<string, string> valueOfAttr;
			for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
				string value;
				tupleStrIn >> value;
				valueOfAttr.insert(pair<string, string>(attrIter->attrname, value));
			}

#if DEBUG_IT
			//            cout << "Map Values:" << endl;
			//            for (auto iter = valueOfAttr.begin(); iter != valueOfAttr.end(); iter++) {
			//                cout << iter->first << ": " << iter->second << endl;
			//            }
#endif

			if (checkInRange(ranges, valueOfAttr)) {
				// TODO: delete
#if DEBUG_IT
				cout << "To be deleted: " << blockNumInFile << " " << blockOffsetInFile << endl;
#endif
				bufferMgr.Delete(blockNum, blockOffsetInFile, size);

				for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
					if (attrIter->indexname != "noindex") {
						updatedKeys.find(attrIter->indexname)->second.type = (DataType)attrIter->type;
						updatedKeys.find(attrIter->indexname)->second.keys.push_back(valueOfAttr.find(attrIter->attrname)->second);
					}
				}
			}
		}

		// update index
		for (auto keys : updatedKeys) {
			string str = keys.first + ".idx";
			bPlusTree.Delete(keys.second.type, &str, keys.second.keys);
		}

#if 0
		// write
		double d = 3.14;
		s.write(reinterpret_cast<char*>(&d), sizeof d); // binary output
		s << 123 << "abc";                              // text output

		// for fstream, this moves the file position pointer (both put and get)
		s.seekp(0);

		// read
		s.read(reinterpret_cast<char*>(&d), sizeof d); // binary input
		int n;
		std::string str;
		if (s >> n >> str)                             // text input
			std::cout << "read back from file: " << d << ' ' << n << ' ' << str << '\n';
#endif
	}
}

bool RecordManager::inIntRange(IntRange *range, int val) {
	if (val < range->minValue || val > range->maxValue)
		return false;
	if (val == range->minValue && !(range->includeMin))
		return false;
	if (val == range->maxValue && !(range->includeMax))
		return false;
	for (auto iter = range->excludeValues.begin(); iter != range->excludeValues.end(); iter++) {
		if (*iter == val)
			return false;
	}

	return true;
}

bool RecordManager::inFloatRange(FloatRange *range, float val) {
	if (val < range->minValue || val > range->maxValue)
		return false;
	if (val == range->minValue && !(range->includeMin))
		return false;
	if (val == range->maxValue && !(range->includeMax))
		return false;
	for (auto iter = range->excludeValues.begin(); iter != range->excludeValues.end(); iter++) {
		if (*iter == val)
			return false;
	}

	return true;
}

bool RecordManager::inStringRange(StringRange *range, string val) {
	if (range->value != "" && val != range->value)
		return false;
	else {
		for (auto iter = range->excludeValues.begin(); iter != range->excludeValues.end(); iter++) {
			if (*iter == val)
				return false;
		}
	}

	return true;
}

bool RecordManager::checkInRange(vector<Range *> &ranges, map<string, string> &valueOfAttr) {
	for (Range *range : ranges) {
		switch (range->type) {
		case int_t: {
			IntRange *intRange = static_cast<IntRange *> (range);
			int val = stoi(valueOfAttr.find(range->attrName)->second);
			if (!inIntRange(intRange, val)) {
				return false;
			}

			break;
		}

		case myFloat: {
			FloatRange *floatRange = static_cast<FloatRange *> (range);
			float val = stof(valueOfAttr.find(range->attrName)->second);
			if (!inFloatRange(floatRange, val)) {
				return false;
			}
			break;
		}

		case char_t: {
			StringRange *stringRange = static_cast<StringRange *> (range);
			string val = valueOfAttr.find(range->attrName)->second;
			if (!inStringRange(stringRange, val)) {
				return false;
			}
			break;
		}
		default:
			break;
		}
		//                vector<Attribute>::iterator attrIter = find_if(tableInfo.Attr.begin(), tableInfo.Attr.end(), [range](Attribute &attribute) {
		//                    return attribute.attrname == range->attrName;
		//                });

	}

	return true;
}

void RecordManager::checkDeleteTupleInBuffer(vector<Range *> &ranges) {
	for (int i = 0; i < tableInfo.blocknum + 1; i++)
	{
		int blockNum = bufferMgr.FindBlockinBuffer(tableName + ".rec", i);
		char *tuplePos = bufferMgr.GetDetail(blockNum, 0);
		istrstream allTupleStrIn(tuplePos);

		string tuple;
		int posBegin = (int)allTupleStrIn.tellg();

		map<string, KeysWithType> updatedKeys; // to be updated according to delete

		for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
			if (attrIter->indexname != "noindex") {
				KeysWithType keys;
				updatedKeys.insert(pair<string, KeysWithType>(attrIter->indexname, keys));
			}
		}

		while (getline(allTupleStrIn, tuple)) {
			int posEnd = (int)allTupleStrIn.tellg();
#if DEBUG_IT
			cout << tuple << endl;
#endif
			if (tuple[0] == ' ')
				continue;

			istrstream tupleStrIn(tuple.c_str());

			map<string, string> valueOfAttr;
			for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
				string value;
				tupleStrIn >> value;
				valueOfAttr.insert(pair<string, string>(attrIter->attrname, value));
			}

			if (checkInRange(ranges, valueOfAttr)) {
				bufferMgr.Delete(blockNum, posBegin, posEnd - posBegin);
				for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
					if (attrIter->indexname != "noindex") {
						updatedKeys.find(attrIter->indexname)->second.type = (DataType)attrIter->type;
						updatedKeys.find(attrIter->indexname)->second.keys.push_back(valueOfAttr.find(attrIter->attrname)->second);
					}
				}
			}
			posBegin = posEnd;
	}

		for (auto keys : updatedKeys) {
			string str = keys.first + ".idx";
			bPlusTree.Delete(keys.second.type, &str, keys.second.keys);
		}
	}
}

string RecordManager::projectTuple(string tuple, vector<string> &values) {
	if (values.size() == 1 && values[0] == "*") {
		return tuple;
	}

	string result;

	istrstream tupleStrIn(tuple.c_str());
	map<string, string> valueOfAttr;
	for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
		string value;
		tupleStrIn >> value;
		valueOfAttr.insert(pair<string, string>(attrIter->attrname, value));
	}

	for (auto valueIter = values.begin(); valueIter != values.end(); valueIter++) {
		result += valueOfAttr.find(*valueIter)->second; // TODO: verify the value attrs
		result += " ";
	}

	return result;
}

void RecordManager::checkSelectTuple(vector<Range *> &ranges, vector<string> &attributes) {
	const string file = "select.tmp";
	std::ifstream posFileStream(file);
	if (!posFileStream.is_open()) {
		std::cerr << "failed to open " << file << '\n';
	}
	else {
		int blockNumInFile, blockOffsetInFile;

		while (posFileStream >> blockNumInFile >> blockOffsetInFile) {
#if DEBUG_IT
			cout << blockNumInFile << " " << blockOffsetInFile << endl;
#endif
			int blockNum;
			blockNum = bufferMgr.FindBlockinBuffer(tableInfo.getName() + ".rec", blockNumInFile);
			char *tuplePos = bufferMgr.GetDetail(blockNum, blockOffsetInFile);
			char *newLineChar = strchr(tuplePos, '\n');
			int size = newLineChar - tuplePos;
			string tuple;
			tuple.assign(tuplePos, size);
#if DEBUG_IT
			cout << "Tuple: " << tuple << endl;
#endif
			istrstream tupleStrIn(tuplePos);
#if 0
			cout << "Tuple size: " << size << endl;
#endif

#if DEBUG_IT
			//            cout << "Tuple: " << tuplePos << endl;
			//            cout << "Tuple stream: " << tupleStrIn.str();
#endif
			map<string, string> valueOfAttr;
			for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
				string value;
				tupleStrIn >> value;
				valueOfAttr.insert(pair<string, string>(attrIter->attrname, value));
			}

#if DEBUG_IT
			//            cout << "Map Values:" << endl;
			//            for (auto iter = valueOfAttr.begin(); iter != valueOfAttr.end(); iter++) {
			//                cout << iter->first << ": " << iter->second << endl;
			//            }
#endif

			if (checkInRange(ranges, valueOfAttr)) {
				cout << projectTuple(tuple, attributes);
				cout << endl;
			}
		}
	}
}

void RecordManager::checkSelectTupleInBuffer(vector<Range *> &ranges, vector<string> &attributes) {
	for (int i = 0; i < tableInfo.blocknum + 1; i++) {
		int blockNum = bufferMgr.FindBlockinBuffer(tableName + ".rec", i); // TODO: Multi block
		char *tuplePos = bufferMgr.GetDetail(blockNum, 0);
		istrstream allTupleStrIn(tuplePos);

		string tuple;
		int posBegin = (int)allTupleStrIn.tellg();

		while (getline(allTupleStrIn, tuple)) {
			int posEnd = (int)allTupleStrIn.tellg();
#if DEBUG_IT
			//        cout << tuple << endl;
#endif
			if (tuple[0] == ' ')
				continue;

			istrstream tupleStrIn(tuple.c_str());

			map<string, string> valueOfAttr;
			for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
				string value;
				tupleStrIn >> value;
				valueOfAttr.insert(pair<string, string>(attrIter->attrname, value));
			}

			if (checkInRange(ranges, valueOfAttr)) {
				cout << projectTuple(tuple, attributes);
				cout << endl;
			}
			posBegin = posEnd;
		}
	}
}

bool RecordManager::checkUnique(Attribute attr, string valueToInsert) {
	if (attr.indexname != "noindex") {
		string indexFile = attr.indexname + ".idx";
		bPlusTree.Find(attr.type, &indexFile, &valueToInsert, &valueToInsert, false, false);
		const string file = "select.tmp";
		std::ifstream posFileStream(file);
		if (!posFileStream.is_open()) {
			std::cerr << "failed to open " << file << '\n';
		}
		else {
			int blockNumInFile, blockOffsetInFile;
			if (posFileStream >> blockNumInFile >> blockOffsetInFile)
				return false;
		}
	}
	else {
		int blockNum = bufferMgr.FindBlockinBuffer(tableName + ".rec", 0); // TODO: Multi block
		char *tuplePos = bufferMgr.GetDetail(blockNum, 0);
		istrstream allTupleStrIn(tuplePos);

		string tuple;

		while (getline(allTupleStrIn, tuple)) {
#if DEBUG_IT
			//        cout << tuple << endl;
#endif
			if (tuple[0] == ' ')
				continue;

			istrstream tupleStrIn(tuple.c_str());

			map<string, string> valueOfAttr;
			for (auto attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
				string value;
				tupleStrIn >> value;
				valueOfAttr.insert(pair<string, string>(attrIter->attrname, value));
			}

			if (valueToInsert == valueOfAttr.find(attr.attrname)->second)
				return false;
		}
	}

	return true;
}

