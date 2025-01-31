#!/usr/bin/env bash


help_msg="
run_ht_scope: Start phoebus gui for ht scope

Usage:
    ./run_ht_scope.sh --host=kamino --user=dt100 acq1102_015 acq1102_010 acq1102_999 acq1102_999

Args:
    uuts         Hostname of uuts to target
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

macros["UUT_1"]="${positional_args[0]:-acq1102_015}" #TODO change default
macros["UUT_2"]="${positional_args[1]:-None}"
macros["UUT_3"]="${positional_args[2]:-None}"
macros["UUT_4"]="${positional_args[3]:-None}"

macros["PFX"]="${macros['HOST']}:${macros['USER']}"


macro_to_query() {
    local query=""

    for key in "${!macros[@]}"; do
        value="${macros[$key]}"
        query+="&${key}=${value}"
    done
    echo "${query:1}"
}

ROOT_DIR=$(dirname "$(dirname "$(realpath "$0")")")
PHOEBUS_JAR="$HOME/PROJECTS/phoebus/phoebus-product/target/product-4.7.4-SNAPSHOT.jar"
JAVA_EXE="java -Dlog.level=DEBUG"
ARGS="-nosplash"
PROTOCOL="file://"
LAUNCHER="$ROOT_DIR/CSS/ht_scope_launcher.bob"
SETTINGS="$ROOT_DIR/CSS/settings.ini"
QUERY=$(macro_to_query)
RESOURCE="${PROTOCOL}${LAUNCHER}?${QUERY}"

rm ~/.phoebus/memento

CMD="$JAVA_EXE -jar $PHOEBUS_JAR $ARGS -resource $RESOURCE -settings $SETTINGS"

echo "CMD: $CMD"
$CMD