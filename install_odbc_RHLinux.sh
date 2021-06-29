sudo yum search unixODBC
sudo yum install -y unixODBC.x86_64 
sudo yum install -y postgresql-odbc.x86_64
sudo dnf install unixODBC-devel
odbcinst -q -d      
odbcinst -j