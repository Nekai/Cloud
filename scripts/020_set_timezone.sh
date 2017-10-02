#!/bin/bash
#
# Set timezone to something American as opposed to default UTC.
#
# https://www.cyberciti.biz/faq/centos-linux-6-7-changing-timezone-command-line/

# Pull in config settings... not really needed for this script, but ordnung!
source $(dirname "$BASH_SOURCE")/000_config.sh

# TZ="America/Denver" 
TZ="America/Indiana/Indianapolis"

timedatectl set-timezone "$TZ"
