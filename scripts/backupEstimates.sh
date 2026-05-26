set -e

cd "factDB/gen/"

mkdir -p "./backups"

backup_file="./backups/estimates-$(date '+%Y%m%dT%H%M').csv"
echo "Creating $backup_file"

umask 337
cp ./estimates.csv "$backup_file"
