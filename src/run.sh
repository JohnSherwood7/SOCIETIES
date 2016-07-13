iterations=4
iterationNum=1
batchID=uuidgen
iterationID=uuidgen

while [ $iterationNum -lt $iterations ]
do
    echo Run $iterationNum >> testDB/output.txt
    ./societies -p testDB/devices_test.conf -z testDB/agentcsv.csv -s testDB/fieldsResults -d $batchID $iterationID $a GS testDB/dbResults -v 0 >> testDB/output.txt
    (( iterationNum++ ))
done 
