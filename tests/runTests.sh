
TEST_DIR="../build/tests"

for file in `ls $TEST_DIR` 
do
	echo "running $TEST_DIR/$file"
	$TEST_DIR/$file
done

