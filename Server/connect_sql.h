#pragma once

#include <sql.h>
#include <sqlext.h>

SQLHENV ConnectToDatabase();
void DisconnectFromDatabase(SQLHDBC hdbc, SQLHENV henv);
