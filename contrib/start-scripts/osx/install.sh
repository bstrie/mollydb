sudo sh -c 'echo "POSTGRESQL=-YES-" >> /etc/hostconfig'
sudo mkdir /Library/StartupItems/MollyDB
sudo cp MollyDB /Library/StartupItems/MollyDB
sudo cp StartupParameters.plist /Library/StartupItems/MollyDB
if [ -e /Library/StartupItems/MollyDB/MollyDB ]
then
  echo "Startup Item Installed Successfully . . . "
  echo "Starting MollyDB Server . . . "
  SystemStarter restart MollyDB
fi
