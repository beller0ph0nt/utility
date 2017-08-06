
TMP_FILE=resp

../build/srv 4444 &
./test_01 192.168.1.66 4444 abracadabra test > $TMP_FILE

cmp -s $TMP_FILE ok_resp
if [ $? -eq 0 ]; then
    echo "OK responce test [ OK ]"
else
    echo "OK responce test [ ERR ]"
fi

rm -f $TMP_FILE