#!/usr/bin/env bash


help_msg="
run_ht_scope_ui: Start phoebus gui for ht scope

Usage:
    ./run_ht_scope_ui.sh --host=kamino --user=dt100

Args:
    --host:      IOC host
    --user:      IOC user
    --chans:     Total chans per uut

"


#Argparser
declare -A named_args
positional_args=()

while [[ "$1" ]]; do
    if [[ "$1" == --* ]]; then
        case "$1" in
            --help)
                echo -e "$help_msg"
                exit 1
                ;;
            --*=*)
                key="${1%%=*}"
                value="${1#*=}"
                named_args["${key#--}"]="$value"
                ;;
            *)
                echo "Unknown option: $1"
                exit 1
                ;;
        esac
    else
        positional_args+=("$1")
    fi
    shift
done

#Macros
declare -A macros

macros["HOST"]="${named_args[host]:-kamino}"
macros["USER"]="${named_args[user]:-dt100}"
macros["NCHAN"]="${named_args[chans]:-32}"

macro_to_query() {
    local query=""

    for key in "${!macros[@]}"; do
        value="${macros[$key]}"
        query+="&${key}=${value}"
    done
    echo "${query:1}"
}

ROOT_DIR=$(dirname "$(dirname "$(realpath "$0")")")
PHOEBUS_PROD="$HOME/PROJECTS/phoebus/phoebus-product/target/"
[ -e $PHOEBUS_PROD/update ] && PHOEBUS_PROD=$PHOEBUS_PROD/update
PHOEBUS_JAR=$(find $PHOEBUS_PROD -name product\*jar)
if [ -z $PHOEBUS_JAR ]; then
	echo ERROR unable to find JAR in $PHOEBUS_PROD
	exit 1
elif [ ! -e $PHOEBUS_JAR ] ;then
	echo ERROR unable to find PHOEBUS_JAR $PHOEBUS_JAR
	exit 1
fi
#PHOEBUS_JAR="$HOME/PROJECTS/phoebus/phoebus-product/target/product-4.7.4-SNAPSHOT.jar"
#PHOEBUS_JAR="$HOME/PROJECTS/phoebus/phoebus-product/target/product-4.7.3-SNAPSHOT.jar"
JAVA_EXE="java -Dlog.level=DEBUG"
ARGS="-nosplash"
PROTOCOL="file://"
LAUNCHER="$ROOT_DIR/phoebus/CSS/ht_scope_launcher.bob"
SETTINGS="$ROOT_DIR/phoebus/CSS/settings.ini"
QUERY=$(macro_to_query)
RESOURCE="${PROTOCOL}${LAUNCHER}?${QUERY}"

rm ~/.phoebus/memento


CMD="$JAVA_EXE -jar $PHOEBUS_JAR $ARGS -resource $RESOURCE -settings $SETTINGS"
echo "CMD: $CMD"
$CMD

