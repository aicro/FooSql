#include <vector>
#include <iostream>
#include <tr1/memory>

#include "dbcomm/DbComm.h"
#include "exception/ThrowableException.h"

using namespace std;
using namespace COMMON::DBCOMM;
using namespace COMMON::EXCEPTION;

int main()
{
	DbLocation dbLocation1;
	dbLocation1.SetDbId("TEST_DB1");
    dbLocation1.SetIp("127.0.0.1");
    dbLocation1.SetPort("3306");
    dbLocation1.SetUser("root");
    dbLocation1.SetPassword("123456");
	
    char str1[20 + 1] = {0};
    memcpy(str1, "abc\0efg\nhijkl'123'\ra", 20);

    try
    {
        vector<DbLocation> dbLocations_array;
        dbLocations_array.push_back(dbLocation1);

        tr1::shared_ptr<IDbTasks> mysqlTasks( new MysqlDbTasks(dbLocations_array, true) );
        mysqlTasks->Connect();

        EscapeStringAction* escape_string_action = (EscapeStringAction*)mysqlTasks->EscapeString();
        EscapeSrtingFilter filter(str1, 20);
        escape_string_action->Do(&filter);

        string escaped_string = escape_string_action->GetEscapedString();
        cout << "The string after escape : [" << escaped_string << "]" << endl;
        
        escape_string_action->EndAction();

        mysqlTasks->Disconnect();
    }
    catch (ThrowableException& e)
    {
        cout << e.What(true) << endl;
    }
    catch (...)
    {
        cout << "unknown exception" << std::endl;
    }
	
	return 0;
}