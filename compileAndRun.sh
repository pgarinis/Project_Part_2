make clean
make
echo "Running with harness now"
cd small
./harness small.init small.work small.result ./../bin/dbsystem
