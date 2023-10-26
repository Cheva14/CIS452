# Project Two

For this project you will be using semaphores, threads and shared memory.

Each baker will run as its own thread. Access to each resource will require a counting/binary semaphore as appropriate.

The program should prompt the user for a number of bakers. Each baker will be competing for the resources to create each of the recipes.

Each baker is in a shared kitchen with the following resources:

Mixer - 2

Pantry – 1

Refrigerator - 2

Bowl – 3

Spoon – 5

Oven – 1

Only one baker may ‘access’ an ingredient at a time.

Ingredients available in the pantry:

1. Flour
2. Sugar
3. Yeast
4. Baking Soda
5. Salt
6. Cinnamon

Ingredients available in the refrigerator:

1. Egg(s)
2. Milk
3. Butter

Only one baker may be in the pantry at a time.

Two bakers may be in the refrigerator at a time.

To ‘bake’ a recipe, the baker must acquire each of the ingredients listed below:

Cookies: Flour, Sugar, Milk, Butter

Pancakes: Flour, Sugar, Baking soda, Salt, Egg, Milk, Butter

Homemade pizza dough: Yeast, Sugar, Salt

Soft Pretzels: Flour, Sugar, Salt, Yeast, Baking Soda, Egg

Cinnamon rolls: Flour, Sugar, Salt, Butter, Eggs, Cinnamon

Once all the ingredients have been acquired: a bowl, a spoon and a mixer must be acquired to mix the ingredients together. After they have been mixed together, it must be cooked in the oven.

Each baker must complete (and cook) each of the recipes once and then announce they have finished.

Each baker must attempt to complete each item as soon as possible (don’t put them to sleep).

Output to the terminal what each of the bakers are doing in real time. The output from each baker should be a different color.

You may use System V semaphores or POSIX semaphores.

Assign one of the bakers to have a chance to be ‘Ramsied’, this means they must release all semaphores and start their current recipe from the beginning. This must be programmed to occur every time the program is run.
