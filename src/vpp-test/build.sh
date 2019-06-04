ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../../env.ini

cd $ROOT_DIR

export LD_LIBRARY_PATH=$VPP_ROOT/vpp/lib:/host/lib64

case $1 in
test)
shift
make -f build.mk VPP_LIB=$VPP_ROOT/vpp/lib VPP_INCLUDE=$VPP_ROOT/vpp/include all
$ROOT_DIR/test_$@
;;
gdb)
shift
make -f build.mk VPP_LIB=$VPP_ROOT/vpp/lib VPP_INCLUDE=$VPP_ROOT/vpp/include all
gdb -ex r $ROOT_DIR/test_$@
;;
*)
make -f build.mk VPP_LIB=$VPP_ROOT/vpp/lib VPP_INCLUDE=$VPP_ROOT/vpp/include $@
esac