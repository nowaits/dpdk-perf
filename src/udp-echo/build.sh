ROOT_DIR=`pushd ${0%/*} >/dev/null && pwd && popd >/dev/null`

source $ROOT_DIR/../../env.ini

cd $ROOT_DIR

make -f build.mk VPP_LIB=$VPP_ROOT/vpp/lib VPP_INCLUDE=$VPP_ROOT/vpp/include $@