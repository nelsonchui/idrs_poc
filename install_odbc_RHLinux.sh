# Install ODBC packages
# sudo yum search unixODBC
sudo yum install -y unixODBC.x86_64 
sudo yum install -y postgresql-odbc.x86_64
sudo dnf install unixODBC-devel
# ODBC command to list driver and config file path
odbcinst -q -d      
odbcinst -j
# install gcc compiler
sudo dnf install gcc
sudo dnf groupinstall "Development Tools"
gcc --version
# install gitHUB
sudo yum install git-core.x86_64
# install Terraform
# sudo dnf install -y terraform
# terraform -help
# download source code from github
git clone https://github.com/nelsonchui/idrs_poc.git