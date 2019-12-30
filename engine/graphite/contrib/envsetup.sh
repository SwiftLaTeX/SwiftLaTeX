
export PRJROOT=`pwd`
if [ ! -e "$PRJROOT/contrib/envsetup.sh" ]
then echo "run . ./contrib/envsetup.sh from the root of the graphite2 tree"
fi
