# Project Instructions

For this project you will be using semaphores, threads and shared memory. Each baker will run as its own thread. Access to each resource will require a counting/binary semaphore as appropriate.

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

# Project **Documentation**

## **Project Overview**

The project aims to simulate a shared kitchen environment where multiple bakers, each running as a separate thread, compete for resources to create various recipes. The simulation involves the use of semaphores for synchronization, shared memory for communication, and dynamic color-coding to visually distinguish between different bakers.

## **Project Components**

The project consists of the following components:

### **1. Baker Threads**

- Each baker is represented by a separate thread.
- Bakers compete for resources such as mixers, pantry items, refrigerator items, bowls, spoons, and the oven.
- One baker is designated randomly as "Ramsied," requiring them to restart one random recipe from the beginning.

### **2. Resources and Semaphores**

- Ingredients (Flour, Sugar, Yeast, Baking Soda, Salt, Cinnamon, Eggs, Milk, and Butter) are located in the pantry and refrigerator.
- Semaphores are used to control access to shared resources:
  - Mixer (Counting Semaphore - 2 permits)
  - Pantry (Binary Semaphore - 1 permit)
  - Refrigerator (Counting Semaphore - 2 permits)
  - Bowl (Counting Semaphore - 3 permits)
  - Spoon (Counting Semaphore - 5 permits)
  - Oven (Binary Semaphore - 1 permit)

### **3. Baking Process**

- Bakers acquire the necessary ingredients and utensils (bowl, spoon, mixer) to mix the ingredients.
- Once mixed, the recipe is cooked in the oven.
- Each baker completes each recipe once and announce their completion.

### **4. Color Management**

- Dynamic color-coding is implemented to visually distinguish between different bakers.
- Each baker's output is displayed in a unique color for real-time tracking.

### **5. Ramsied Mechanisn**

- One baker is randomly designated as "Ramsied" at the start of the program.
- Ramsied baker releases all acquired semaphores and restarts a recipe randomly.

## **Implementation Details**

### **1. Baking Threads (baker_thread function)**

- Each baker thread follows a similar process for creating different baked goods.
- Bakers acquire ingredients and utensils, perform mixing, and use the oven to bake the goods.
- "Ramsied" baker redo a random recipe.

### **2. Color Management (getColor function)**

- The getColor function generates a random color for each baker, ensuring visual distinction.
- It avoids repeating colors for different bakers.

### **3. Main Function (main function)**

- Collects user input to determine whether the terminal supports RGB colors.
- Takes user input for the number of bakers participating in the simulation.
- Creates threads for each baker, initializing semaphores and baker-specific data.
- Waits for all baker threads to finish.
- Cleans up semaphores after the simulation.

## **Compilation and Execution**

- The program can be compiled using either System V or POSIX semaphores, depending on user preference.
- Compilation command and execution remain consistent regardless of the chosen semaphore type.

`# Compilation with System V semaphores`

`gcc -o bake bakeoff.c -lpthread`

`# Compilation with POSIX semaphores`

`gcc -o bake bakeoff.c -lpthread -lrt`

`# Execution`

`./bake`

## **Conclusion**

The project simulates a shared kitchen environment with multiple bakers using threads, semaphores, and shared memory. The implementation ensures proper synchronization and resource management during the baking process. The inclusion of dynamic color-coding enhances the visual representation of the simulation, making it easier to track the real-time activities of each baker. The Ramsied mechanism adds an element of unpredictability to the simulation, requiring bakers to adapt to occasional challenges.
