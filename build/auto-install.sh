cd ..
dotnet publish -c release -r ubuntu.18.04-x64 --self-contained
mkdir /opt/nkm
cp ./nkm/bin/Release/netcoreapp3.1/ubuntu.18.04-x64/publish/* /opt/nkm
chmod +x /opt/nkm/nkm
ln -s /opt/nkm/nkm /usr/local/bin/nkm
