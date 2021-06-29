# compile script
git clone https://github.com/nelsonchui/idrs_poc.git
cd idrs_poc
mkdir bin
gcc -o ./bin/myODBCConnect ./src/myODBCConnect.c -lodbc
./bin/myODBCConnect