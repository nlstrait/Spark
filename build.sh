xcode-select --install

if xcodebuild ; then
    echo "Xcode command line tools already enabled."
else
    echo "Xcode command line tools not enabled yet, enabling now."
    sudo xcode-select --switch /Library/Developer/CommandLineTools
fi

cd ./Builds/MacOSX

xcodebuild -project Spark.xcodeproj

./build/debug/Spark.app/Contents/MacOS/Spark
