APPNAME="YGOPro"

if [ -z "${NDK_ROOT+aaa}" ];then
echo "please define NDK_ROOT first"
exit 1
fi

DIR="$( cd "$(dirname "${BASH_SOURCE[0] }" )" && pwd )"

APP_ROOT="$DIR/.."
APP_ANDROID_ROOT="$DIR"

echo "APP_ROOT = $APP_ROOT"
echo "NDK_ROOT = $NDK_ROOT"
echo "APP_ANDROID_ROOT = $APP_ANDROID_ROOT"

if [ -d "$APP_ANDROID_ROOT"/assets ]; then
rm -rf "$APP_ANDROID_ROOT"/assets
fi

mkdir "$APP_ANDROID_ROOT"/assets

#copy resources
cp -f "$APP_ROOT"/common.xml  "$APP_ANDROID_ROOT"/assets
cp -rf "$APP_ROOT"/conf "$APP_ANDROID_ROOT"/assets/conf

#run ndk-build
echo "start building..."
"$NDK_ROOT"/ndk-build -C "$APP_ANDROID_ROOT" $*

