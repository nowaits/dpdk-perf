ROOT_DIR=`realpath $(dirname $0)`

source $ROOT_DIR/../../env.ini

cd $ROOT_DIR

make -f build.mk VPP_LIB=$VPP_ROOT/vpp/lib VPP_INCLUDE=$VPP_ROOT/vpp/include $@