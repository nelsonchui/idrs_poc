APP_DIR=$HOME/idrs_poc
SRC_DIR=$APP_DIR/src
BIN_DIR=$APP_DIR/bin

PGM="myODBCInsertRecord"
PGMSRC=$PGM".c"

gcc -o $BIN_DIR/$PGM $SRC_DIR/$PGMSRC -lodbc

echo "Program "$PGM" is built."