
#!/bin/bash

# Get a list of semaphore IDs
semaphore_list=$(ipcs -s | awk 'NR > 3 { print $2 }')

# Loop through the list and remove each semaphore
for semaphore_id in $semaphore_list; do
    ipcrm -s $semaphore_id
done