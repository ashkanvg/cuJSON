#!/bin/bash -l

# Notes:
# Must have cuda installed (tested with toolkit 12.8)
# Must have GPU enabled machine (tested on H100)

module purge
module load cuda/12.8

# Download GraalVM
if [ ! -d "graalvm-ce-java8-21.0.0.2" ]; then
  wget https://github.com/graalvm/graalvm-ce-builds/releases/download/vm-21.0.0.2/graalvm-ce-java8-linux-amd64-21.0.0.2.tar.gz
  tar xvf graalvm-ce-java8-linux-amd64-21.0.0.2.tar.gz
  rm graalvm-ce-java8-linux-amd64-21.0.0.2.tar.gz 
fi

export JAVA_HOME=$PWD/graalvm-ce-java8-21.0.0.2
export GRAALVM_HOME=$PWD/graalvm-ce-java8-21.0.0.2
export PATH=$PWD/graalvm-ce-java8-21.0.0.2/bin:$PATH

which java
java -version

if [ ! -d "gpjson" ]; then
  git clone https://github.com/koesie10/gpjson
fi
cd gpjson
./gradlew copyToGraalVM -PgraalVMDirectory=$GRAALVM_HOME
cd ..

echo -e "\n\n\n"
echo "gpjson was installed"
echo "Due to GraalVM being a dependency, modify the following environment variables:"
echo "export JAVA_HOME=$PWD/graalvm-ce-java8-21.0.0.2"
echo "export GRAALVM_HOME=$PWD/graalvm-ce-java8-21.0.0.2"
echo "export PATH=$PWD/graalvm-ce-java8-21.0.0.2/bin:\$PATH"
echo -e "\n\n"
echo "This was tested on a machine using CUDA 12.8 and an H100 GPU"
