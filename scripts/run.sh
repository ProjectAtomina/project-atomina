#!/usr/bin/env bash

EXECPATH=$(pwd)
SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
  SOURCE=$(readlink "$SOURCE")
  [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

while :; do
    case $1 in
        -a) #app-type [Server|OpenGL]
            if [ -n "$2" ]; then
                case $2 in
                    Server)
                        ;;
                    OpenGL)
                        ;;
                    *)
                        printf "$2 is not a valid app type\n"
                        exit 1
                esac
                apptype=$2
                shift
            else
                printf "app type parameter must have app type argument [Server|OpenGL]\n"
                exit 1
            fi
            ;;
        -b) #build-type [Release|Debug]
            if [ -n "$2" ]; then
                case $2 in
                    Debug)
                        ;;
                    Release)
                        ;;
                    *)
                        printf "$2 is not a valid build type\n"
                        exit 1
                esac
                buildtype=$2
                shift
            else
                printf "build type parameter must have build type argument [Release|Debug]\n"
                exit 1
            fi
            ;;
        -p) #project [Game|Test]
            if [ -n "$2" ]; then
                case $2 in
                    Game)
                        ;;
                    Test)
                        ;;
                    *)
                        printf "$2 is not a valid project type\n"
                        exit 1
                esac
                project=$2
                shift
            else
                printf "build type parameter must have project type argument [Game|Test]\n"
                exit 1
            fi
            ;;
        -?) #unknown arg
            printf "unknown argument $1\n"
            ;;
        *)
            break
    esac
    shift
done

cd $DIR/../
cd out/build/$buildtype/$apptype
stat bin/Atomina_$project
./bin/Atomina_$project
cd $EXECPATH
