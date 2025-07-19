//The libraries in C that support the functions implemented
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


//Maximum student registration limit.
#define maxlimit 100

int containsOnlyLetters(const char *str);
void addNewStudent(void);
void menu(void);
void createFile(void);
void listAllStudents(void);
int compareStudents(const void *a, const void *b);
void searchEditDeleteStudent(void);
void sortStudents(void);
int getValidatedGrade(const char* prompt, int min_val, int max_val, const char* grade_name);

//Each student's information is stored using a struct.
typedef struct {
    int assignment;
    int midterm;
    int final;
    float gpa;
    char name[51]; // Student's full name (max 50 characters + null)
    int id;
}student; //Name of the data type "student".


//Beginning of the main function.
int main(void)
{
    //User's choice.
    int choice;

    do{
        menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a valid number.\n");
            while (getchar() != '\n');//Clears the entered incorrect leftover (junk) characters until a newline character ('\n') is encountered.
            continue;
        }
        while (getchar() != '\n');//Clears the entered incorrect leftover (junk) characters until a newline character ('\n') is encountered.

        //Functions to be used after user input.
        switch(choice){
            case 1: createFile();
            break;
            case 2: addNewStudent();
            break;
            case 3: listAllStudents();
            break;
            case 4: searchEditDeleteStudent();
            break;
            case 5: sortStudents();
            break;
            case 6: puts("Exiting the program...");
            break;
            default: puts("Invalid option! Please try again.");
        }
    }
    while (choice != 6);//Continue until the user enters 6.

    return 0;
}

//Main menu function.
void menu(void){
    puts("\n############################################");
    puts("#                  *MENU*                  #");
    puts("############################################");
    puts(" 1. Create a File");
    puts(" 2. Add Student");
    puts(" 3. Sort students by ID ");
    puts(" 4. Search, Edit, and Delete Student Records");
    puts(" 5. Sort Students by GPA (Ascending)");
    puts(" 6. Exit");
    printf(" Please enter a selection: ");
    puts("\n############################################");
}

//File creation function.
void createFile(void) {

    FILE *studentsfile = fopen("students.txt", "wb");

    if (studentsfile == NULL) {
        puts("File could not be created!");
        return;
    }

    //Default values for student data when the file is created.
    student defaultstudent;
    defaultstudent.midterm=0;
    defaultstudent.final=0;
    defaultstudent.assignment=0;
    defaultstudent.gpa=0;
    defaultstudent.id=0;
    strcpy(defaultstudent.name, "");

    //It writes these empty student records to the file up to the maxlimit (100).
    for (int i = 0; i < maxlimit; i++) {
        fwrite(&defaultstudent, sizeof(student), 1, studentsfile);
    }

    fclose(studentsfile);
    puts("File successfully created.");
    }

//New student entry function.
void addNewStudent(void) {

    FILE *file = fopen("students.txt", "rb+");
    if (file == NULL) {
        puts("File not found. Please create the file first!");
        return;
    }

    //Struct information assigned to the new student to be entered.
    student newstudent;
    int tempID;
    char tempName[51];
    int midterm, assignment, final;


    //Start of the name input section and validation steps.
    int isValidName = 0;
    do {
        printf("Enter the full name of the new student (50 characters): ");
        // Read input from the user into tempName.
        fgets(tempName, sizeof(tempName), stdin);
        // Replace the newline character '\n' at the end (if present) with null terminator '\0'.
        tempName[strcspn(tempName, "\n")] = '\0';
        if (strlen(tempName) == 0) {
            puts("Name cannot be empty. Please enter a name.");
            continue;
        }

        //To determine if the entered value contains only letters, the containsOnlyLetters function comes into play.
        if (containsOnlyLetters(tempName)) {
            isValidName = 1;
        } else {
            puts("Invalid name. Please enter only letters (A-Z, a-z) and spaces.");
        }
    } while (!isValidName);

    //Student ID Input and Validation.
    int isValidID = 0;
    do {
        //User input for ID and validation/control stage of the entered value
        tempID = getValidatedGrade("Enter the Student ID (0-100): ", 0, 100, "Student ID");

        student existingstudent;
        //Move the file pointer to the position where the student with tempID would be located
        fseek(file, tempID * sizeof(student), SEEK_SET);
        // Read the student record from the file into existingStudent
        fread(&existingstudent, sizeof(student), 1, file);
        //Check if a student with this ID already exists
        if (existingstudent.id != 0 && existingstudent.id == tempID) {
            puts("Error! A student with this ID already exists! Please enter a different ID.");

        } else {
            isValidID = 1;
        }
    } while (!isValidID);


    //Midterm, Assignment, and Final Grade Input and Validation.
    midterm = getValidatedGrade("Enter the Midterm grade (0-100): ", 0, 100, "midterm grade");
    assignment = getValidatedGrade("Enter the Assignment grade (0-100): ", 0, 100, "assignment grade");
    final = getValidatedGrade("Enter the Final grade (0-100): ", 0, 100, "final grade");

    //Section for saving the retrieved temporary values to newstudent.
    strcpy(newstudent.name, tempName);
    newstudent.id = tempID;
    newstudent.midterm = midterm;
    newstudent.assignment = assignment;
    newstudent.final = final;
    newstudent.gpa = (midterm * 0.40f) + (assignment * 0.10f) + (final * 0.50f);

    // Move the file pointer to the correct position based on the new student's ID
    fseek(file, newstudent.id * sizeof(student), SEEK_SET);
    //Write the new student's data to the file
    fwrite(&newstudent, sizeof(student), 1, file);

    fclose(file);
    puts("The student successfully added.");
}

//List students function.
void listAllStudents(void) {

    FILE *file = fopen("students.txt", "rb");
    if (file == NULL) {
        puts("File could not be opened. Please create the file first.");
        return;
    }

    //The variables for the existing students to be listed were designated as 's'.
    student s;
    puts("############################################");
    puts("\n############List of All Students############");
    int foundstudents = 0;  // Counter for how many valid student records are found.
    for (int i = 0; i < maxlimit; i++) {
        // Move the file pointer to the ith student record.
        fseek(file, i * sizeof(student), SEEK_SET);
        // Read one student record into variable 's'.
        fread(&s, sizeof(student), 1, file);
        if (s.id != 0) {
            printf("Student ID: %d\n", s.id);
            printf("Student Name: %s\n", s.name);
            printf("Midterm: %d, Assignment: %d, Final: %d\n", s.midterm, s.assignment, s.final);
            printf("GPA: %.2f\n", s.gpa);
            puts("############################################");
            foundstudents++;
        }
    }

    if (foundstudents == 0) {
        puts("There are no student records yet.");
    }
    fclose(file);
}

//Function to edit variables of a student at the given ID number.
void searchEditDeleteStudent(void) {

    FILE *file = fopen("students.txt", "rb+");
    if (file == NULL) {
        puts("File not found. Please create the file first.");
        return;
    }

    //Getting student number input to be searched.
    int searchid;
    printf("Enter the student ID to search: ");
    if (scanf("%d", &searchid) != 1 || searchid < 0 || searchid > 100) {
        puts("Invalid Student ID!");
        while (getchar() != '\n');//Clears the entered incorrect leftover (junk) characters until a newline character ('\n') is encountered.
        fclose(file);
        return;
    }
    while (getchar() != '\n');//Clears the entered incorrect leftover (junk) characters until a newline character ('\n') is encountered.

    student s; //The variables for the existing students to be searched for editing were designated as 's'.
    int found = 0;// Flag to track if student was found.

    // Move file pointer to the correct record and read it.
    fseek(file, searchid * sizeof(student), SEEK_SET);
    fread(&s, sizeof(student), 1, file);

    //If the searched student is found, it is asked if editing is desired for this student.
    if (s.id == searchid) {
        found = 1;
        puts("\n--- Student Information ---");
        printf("Student ID: %d\n", s.id);
        printf("Student Name: %s\n", s.name);
        printf("Midterm: %d, Assignment: %d, Final: %d\n", s.midterm, s.assignment, s.final);
        printf("GPA: %.2f\n", s.gpa);
        puts("--------------------");

        printf("What would you like to do? (E:Edit, D:Delete, B:Back): ");
        char choice;
        scanf(" %c", &choice);
        while (getchar() != '\n');//Clears the entered incorrect leftover (junk) characters until a newline character ('\n') is encountered.

        if (choice == 'E' || choice == 'e') {
            char tempName[51];
            int midterm, assignment, final;


            printf("Enter new Full Name (%s): ", s.name);
            fgets(tempName, sizeof(tempName), stdin);
            tempName[strcspn(tempName, "\n")] = 0;
            if (strlen(tempName) > 0) strcpy(s.name, tempName);

            //Name Entry and Validation Section
            int isValidName = 0;
            do {
                printf("Please enter a valid name: ");

                fgets(tempName, sizeof(tempName), stdin);
                tempName[strcspn(tempName, "\n")] = '\0';


                if (strlen(tempName) == 0) {
                    puts("Name cannot be empty. Please enter a name.");
                    continue;
                }

                //To determine if the entered value contains only letters, the containsOnlyLetters function comes into play.
                if (containsOnlyLetters(tempName)) {
                    isValidName = 1;
                } else {
                    puts("Invalid name. Please enter only letters (A-Z, a-z) and spaces.");
                }
            } while (!isValidName);


            //Midterm, Assignment, and Final Grade Input and Validation.
            midterm = getValidatedGrade("Enter the Midterm grade (0-100): ", 0, 100, "midterm grade");
            assignment = getValidatedGrade("Enter the Assignment grade (0-100): ", 0, 100, "assignment grade");
            final = getValidatedGrade("Enter the Final grade (0-100): ", 0, 100, "final grade");
            s.gpa = (s.midterm * 0.40f) + (s.assignment * 0.10f) + (s.final * 0.50f);


            // Write updated record back to file.
            fseek(file, searchid * sizeof(student), SEEK_SET);
            fwrite(&s, sizeof(student), 1, file);
            puts("Student information successfully updated.");

        //Converting deleted students to an empty default student.
        } else if (choice == 'D' || choice == 'd') {
            student emptyStudent;
            emptyStudent.id = 0;
            emptyStudent.midterm = 0;
            emptyStudent.assignment = 0;
            emptyStudent.final = 0;
            emptyStudent.gpa = 0.0f;
            strcpy(emptyStudent.name, "");

            fseek(file, searchid * sizeof(student), SEEK_SET);
            fwrite(&emptyStudent, sizeof(student), 1, file);
            puts("Student record successfully deleted.");

        } else {
            puts("Delete operation cancelled.");
        }
    }

    if (!found) {
        puts("Student not found.");
    }
    fclose(file);
}

// This function is used to compare the GPAs of two students
// and sorts the students in descending order based on their GPA (Ascending).
int compareStudents(const void *a, const void *b) {
    student *studentA = (student *)a;
    student *studentB = (student *)b;
    if (studentA->gpa < studentB->gpa) return 1;
    if (studentA->gpa > studentB->gpa) return -1;
    return 0;
}

//This function sorts the students by their GPA.
void sortStudents(void) {

    FILE *file = fopen("students.txt", "rb");
    if (file == NULL) {
        puts("File could not be opened. Please create the file first!");
        return;
    }

    student studentsArray[maxlimit];
    int actualStudentCount = 0;

    // Read students from file and count valid entries.
    for (int i = 0; i < maxlimit; i++) {
        fread(&studentsArray[i], sizeof(student), 1, file);
        if (studentsArray[i].id != 0) {
            actualStudentCount++;
        }
    }
    fclose(file);

    if (actualStudentCount == 0) {
        puts("No student records available for sorting.");
        return;
    }

    // Copy valid student records into a temporary array.
    student tempStudents[actualStudentCount];
    int j = 0;
    for (int i = 0; i < maxlimit; i++) {
        if (studentsArray[i].id != 0) {
            tempStudents[j++] = studentsArray[i];
        }
    }

    // Sort the array by GPA in descending order.
    qsort(tempStudents, actualStudentCount, sizeof(student), compareStudents);


    // Display sorted students
    puts("\n### Students Sorted by GPA (Descending) ###");
    for (int i = 0; i < actualStudentCount; i++) {
        printf("Student ID: %d, Student Name: %s, GPA: %.2f\n", tempStudents[i].id, tempStudents[i].name, tempStudents[i].gpa);
    }
    puts("####################");

    // Ask the user if they want to save the result to a file.
    printf("Do you want to save the sorted list to a separate file? (Y/N): ");
    char choice;
    scanf(" %c", &choice);
    while (getchar() != '\n');

    if (choice == 'Y' || choice == 'y') {
        FILE *sortedFile = fopen("sorted_students.txt", "w");
        if (sortedFile == NULL) {
            puts("Sorted file could not be created..");
            return;
        }
         // Write sorted students to the file
        for (int i = 0; i < actualStudentCount; i++) {
            fprintf(sortedFile, "Student ID: %d, Student Name: %s, Midterm: %d, Assignment: %d, Final: %d, GPA: %.2f\n",
                    tempStudents[i].id, tempStudents[i].name,
                    tempStudents[i].midterm, tempStudents[i].assignment,
                    tempStudents[i].final, tempStudents[i].gpa);
        }
        fclose(sortedFile);
        puts("The sorted list has been saved to 'sorted_students.txt'.");
    }
}

//Function for entering and validating student midterm, final, assignment, and ID values.
int getValidatedGrade(const char* prompt, int min_val, int max_val, const char* value_name) {

    int value = 0;
    char inputBuffer[100];
    int isValid = 0;

    do {
        printf("%s", prompt);

        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
            puts("Error reading input. Please try again.");
            continue;
        }
        // Remove newline character.
        inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

        if (strlen(inputBuffer) == 0) {
            printf("Input cannot be empty. Please enter a number for the %s.\n", value_name);
            continue;
        }

        char extra_char;
        int scan_count = sscanf(inputBuffer, "%d %c", &value, &extra_char);  // Check for extra characters.

        if (scan_count == 1) {
            if (value >= min_val && value <= max_val) {
                isValid = 1;  // Valid number within range
            } else {
                printf("Invalid %s! Please enter a value between %d and %d.\n", value_name, min_val, max_val);
            }
        } else if (scan_count == 0) {
            printf("Invalid input. Please enter a number for the %s.\n", value_name);
        } else {
            printf("Invalid input. Please enter only a number for the %s, no extra characters.\n", value_name);
        }

    } while (!isValid);  // Loop until valid input is entered.
    return value;  // Return the validated value.
}

//Function to check if the entered name contains only letters during add student and edit operations.
int containsOnlyLetters(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        //It continues as long as there is a conditional space in the entered name.
        if (str[i] == ' ') {
            continue;
        }
        //Returns a non-zero value (1) if the name contains characters other than letters and spaces.
        if (!isalpha(str[i])) {
            return 0;
        }
    }
    return 1;
}
