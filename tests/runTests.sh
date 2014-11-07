
TEST_DIR="../build/tests"

for file in `find $TEST_DIR -perm -g=x -type f`
do
	echo "running $file"
	$file
done

