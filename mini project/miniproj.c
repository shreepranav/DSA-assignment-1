#include <stdio.h>
#include <stdlib.h>

#define MAX_COURSES 20
#define MAX_STUDENTS 100

#define MAX_COURSES_PER_STUDENT 6
#define MAX_COURSE_STRENGTH 99

typedef struct Course
{
    char *course_name;
    int course_code;
    int credits;
} Course;

typedef struct CoursePerf
{
    Course *course;
    int grade;
    struct CoursePerf *next;
} CoursePerf;

typedef struct Student
{
    char *name;
    int num_courses;
    CoursePerf *course_list;
} Student;

Course *courses_offered[MAX_COURSES] = {NULL};
int courselist_len = 0;
Student *students[MAX_STUDENTS] = {NULL};
int studlist_len = 0;

// Compares two strings 's1' and 's2'.
// Return -1 if s1 is smaller, 1 if s2 is smaller and 0 if they are equal
int strcmp(const char *s1, const char *s2)
{
    if (s1 == NULL)
        return (s2 == NULL) ? 0 : -1;
    if (s2 == NULL)
        return 1;
    int i;
    for (i = 0; s1[i] != '\0'; i++)
    {
        if (s2[i] < s1[i])
            return 1;
        if (s1[i] < s2[i])
            return -1;
    }
    if (s2[i] != '\0')
        return -1;
    return 0;
}

// Copies the string pointed to by 'src', including the terminating null byte ('\0'),
// to the buffer pointed to by 'dest'. At most 'n' bytes of 'src' are copied.
// Warning: If there is no null byte among the first 'n' bytes of 'src', the string
// placed in 'dest' will not be null-terminated.
char *strcopy(const char *src)
{
    int i;
    for (i = 0; src[i] != '\0'; i++)
        ;
    char *dest = (char *) malloc(i * sizeof(char));
    for (; i > -1; i--)
        dest[i] = src[i];
    return dest;
}

void add_course(char *name, int code, int credits)
{
    if (courselist_len == MAX_COURSES)
    {
        printf("Too many courses!\n");
        return;
    }
    // allocate memory for the new Course node
    Course *new_course = (Course *) malloc(sizeof(Course));
    new_course->course_name = strcopy(name);  //TODO: the course_name in the struct either needs to be an array and copy 'name' into that OR memory should be allocated here for 'name', copy that 'name' into newly create memory and save pointer to this newly created memory in the struct as char* 
    new_course->course_code = code;
    new_course->credits = credits;
    courses_offered[courselist_len++] = new_course;
}

void free_courses()
{
    int i;
    for (i = 0; i < courselist_len; i++)
        free(courses_offered[i]);
}

Course* find_course(int code)
{
    int i;
    for (i = 0; i < courselist_len; i++)
        if (courses_offered[i]->course_code == code)
            return courses_offered[i];
    return NULL;
}

Student* find_student(char* name)
{
    int i;
    for(i = 0; i < studlist_len; i++)
        if (strcmp(name, students[i]->name) == 0)
            return students[i];
    return NULL;
}

//TODO: while adding a course to student, grade will always be 'U'. Hence do not take the grade as input, always just assign 'U' in this function
CoursePerf* add_course_to_student(CoursePerf** h, int course_code, int grade)
{
    //TODO: first call find_course and if it returns NULL, return properly from this function
    int credits = find_course(course_code)->credits;
    CoursePerf *new_node = (CoursePerf *) malloc(sizeof(CoursePerf));
    new_node->course = find_course(course_code);
    new_node->grade = grade;
    CoursePerf *p = *h;
    if (*h == NULL)
    {
        new_node->next = NULL;
        *h = new_node;
        return new_node;
    }
    while(p->next && p->next->course->credits > credits)
        p = p->next;
    // TODO: you don't need 'temp' variable below. You can use the below snippet instead:
    /*
    new_node->next = p->next;
    p->next = new_node;
    */
    CoursePerf *temp = p->next;
    p->next = new_node;
    new_node->next = temp;
    return new_node;
}

void add_student(char *name, int course_code)
{
    Student *new_student = (Student *) malloc(sizeof(Student));
    new_student->name = name; // TODO: do the same for here as suggested for the Course.name in the add_course function
    new_student->num_courses = 1;
    new_student->course_list = NULL;
    add_course_to_student(&(new_student->course_list), course_code, 11);
    students[studlist_len++] = new_student;
}

const char* GRADES[] = {"F", "D", "C", "B", "B+", "A", "A+", "U"};

const char* num_to_grade(int g)
{
    if (g < 0 || g > sizeof(GRADES)/sizeof(const char*) + 3 || g > 0 && g < 5)
        return "INVLAID_GRADE";
    return GRADES[g == 0 ? 0 : g - 4];
}

int grade_to_num(char *grade)
{
    int result = -1;
    int g;
    for (g = 0; g < sizeof(GRADES)/sizeof(const char*); ++g)
        if (strcmp(GRADES[g], grade) == 0)
            result = (g == 0) ? 0 : g + 4;
    return result;
}

int grade_to_num2(char *grade)
{
    if (strcmp(grade, "A+") == 0)
        return 10;
    else if (strcmp(grade, "A") == 0)
        return 9;
    else if (strcmp(grade, "B+") == 0)
        return 8;
    else if (strcmp(grade, "B") == 0)
        return 7; 
    else if (strcmp(grade, "C") == 0)
        return 6;
    else if (strcmp(grade, "D") == 0)
        return 5;
    else if (strcmp(grade, "F") == 0)
        return 0;
    else if (strcmp(grade, "U") == 0)
        return -1;
    else
        return -2;   
}

int is_stud_in_course_int(char *student, int course_num)
{
    Student *s = find_student(student);
    // TODO: what if 's' is NULL here. find_student can return a null. So check for that here
    CoursePerf *p = NULL;
    for (p = s->course_list; p != NULL; p = p->next)
    {
        if (p->course->course_code == course_num)
            return 1;
    }
    return 0;
}

// Query 1: GET ALL STUDENTS REGISTERED FOR THE COURSE <course number>
void students_for_course(int course_number)
{
    printf("Here's all the students for course %d:\n", course_number);
}

// Query 2: GET ALL COURSES <student name> HAS REGISTERED FOR
void courses_for_student(char *student_name)
{
    printf("Here's all the courses %s has registered for:\n", student_name);
}

// Query 3: GET ALL STUDENTS REGISTERED FOR BOTH THE COURSES <course number> AND <course number>
void students_for_both_courses(int course_num1, int course_num2)
{
    printf("Here's all the students that registered for both %d and %d:\n", course_num1, course_num2);
}

// Query 4: GET NUMBER OF STUDENTS REGISTERED FOR <course number>
void num_stud_for_course(int course_num)
{
    printf("Here's number of students in course %d:\n", course_num);
}

// Query 5: REGISTER STUDENT <student name> FOR THE COURSE <course number>
void register_for_course(char *student_name, int course_num)
{    
    Student *s = find_student(student_name);
    if(s->num_courses == 6)
    {
        printf("ERROR: Student has already registered for max courses!");
        return;
    }
    add_course_to_student(&(s->course_list), course_num, 11);
    s->num_courses++;
    printf("Registered %s for course %d\n", student_name, course_num);
}

// Query 6: UNREGISTER STUDENT <student name> FOR THE COURSE <course number>
void unregister_for_course(char *student, int course_num)
{
    printf("Unregistered %s for course %d\n", student, course_num);
}

// Query 7: HAS STUDENT <student name> REGISTERED FOR THE COURSE <course number> ?
void is_stud_in_course(char *student, int course_num)
{
    if(is_stud_in_course_int(student, course_num) == 1)
        printf("Yes\n");
    else
        printf("No\n");
}

// Query 8: GET ALL STUDENTS WHO HAVE REGISTERED FOR MORE THAN <number> COURSES
void students_n_courses(int num_courses)
{
    if (num_courses > MAX_COURSES_PER_STUDENT)
    {
        printf("Students do not regiser for more than %d courses!", MAX_COURSES_PER_STUDENT);
        return;
    }

    printf("Here's all the students registered for more than %d courses:\n", num_courses);
}

// Query 9: GET A LIST OF ALL STUDENTS
void print_students()
{
    printf("Here is a list of all students:\n");
    int i;
    for (i = 0; i < studlist_len; i++)
        printf("%s\n", students[i]->name);
}

// Query 10: GET A LIST OF ALL COURSES
void print_courses()
{
    printf("Here is a list of all courses:\n");
    int i;
    for (i = 0; i < courselist_len; i++)
        printf("%d, %s, %d\n", courses_offered[i]->course_code, courses_offered[i]->course_name, courses_offered[i]->credits);
}

// Query 11: GET ALL COURSES WHICH HAVE MORE THAN <number> STUDENTS
void courses_n_students(int strength)
{
    if (strength > MAX_COURSE_STRENGTH)
    {
        printf("More than %d students are not registered for a course!", MAX_COURSE_STRENGTH);
        return;
    }

    printf("Here's all courses with at least %d students:\n", strength);
}

// Query 12: GET ALL <number> CREDIT COURSES <student name> HAS REGISTERED FOR
void n_cred_courses_for_stud(int number, char *student)
{
    if (number < 1 || number > 5)
    {
        printf("Invalid number of credits for a course: %d", number);
        return;
    }

    printf("Here's all the %d credit courses %s has registered for:\n", number, student);
}

// Query 13: GET ALL <number> CREDIT COURSES
void n_cred_courses(int number)
{
    if (number < 1 || number > 5)
    {
        printf("Invalid number of credits for a course: %d", number);
        return;
    }

    printf("Here's all %d credit courses:\n", number);
}

// Query 14: GET THE TOTAL NUMBER OF CREDITS <student name> REGISTERED FOR
void credits_student(char *student)
{
    Student *s = find_student(student);
    CoursePerf *p = NULL;
    int credits = 0;
    for (p = s->course_list; p != NULL; p = p->next)
        credits += p->course->credits;
    printf("Here's the total number of credits %s has taken:\n", student);
}

// Query 15: GET ALL STUDENTS WHO REGISTERED FOR AT LEAST <number> CREDITS
void at_least_n_creds(int number)
{
    if (number > 30)
    {
        printf("Invalid number of credits a student can register for: %d", number);
        return;
    }

    printf("Here's the list of all students taking at least %d credits:\n", number);
}

// Query 16: GET A LIST OF GRADES AND THE GPA OF STUDENT <student name>
void student_grades(char *student)
{
    printf("Here's a list of grades and GPA of %s:\n", student);
}

// Query 17: GET A LIST OF ALL STUDENTS WHO HAS A GPA BELOW <number>
void students_below_gpa(float grade)
{
    if (grade < 0 || grade > 10)
    {
        printf("Invalid gpa: %g", grade);
        return;
    }

    printf("Here's a list of students with GPA below %g:\n", grade);
}

// Query 18: GET A LIST OF ALL STUDENTS WHO HAS A GPA ABOVE <number>
void students_above_gpa(float grade)
{
    if (grade < 0 || grade > 10)
    {
        printf("Invalid gpa: %g", grade);
        return;
    }

    printf("Here's a list of students with GPA above %g:\n", grade);
}

// Query 19: GET A LIST OF ALL STUDENTS WHOSE GRADE IS <grade> AND ABOVE IN <course number>
void students_passing_course(char *grade, int course_num)
{
    printf("Here's a list of students with grade %s or above in course %d:\n", grade, course_num);
}

// Query 20: GET A LIST OF ALL STUDENTS WHOSE GRADE IS BELOW <grade> IN <course number>
void students_failing_course(char *grade, int course_num)
{
    printf("Here's a list of students with grade below %s in course %d:\n", grade, course_num);
}

// Query 21: ASSIGN GRADE <grade> TO STUDENT <student name> FOR THE COURSE <course number>
void assign_grade(char *grade, char *student, int course_num)
{
    printf("Assigned %s to %s in course %d\n", grade, student, course_num);
}

// Query 22: ASSIGN GRADES FOR THE COURSE <course number>
void assign_grades(int course_num)
{

}

// Initialize Courses
// Reads one course per line. (format: <course name>, <course title string>, <credits>)
// Empty line ends the query
void initialize_courses()
{
    printf("ENTER THE COURSE LIST:\n");
    char line[1024] = {'S', 0}; // initializing to a non-empty string
    int cid;
    char title[1024];
    int credits;
    int courses_added = 0, invalid_courses = 0;

    while (scanf("%[^\n]", &line) > 0)
    {
        printf("Line: %s\n", line);

        if(3 == sscanf(line, "%d, %[^,], %d", &cid, title, &credits))
        {
            printf("Adding Course '%s' with id %d and %d credits\n", title, cid, credits);
            ++courses_added;
        }
        else
        {
            printf("*** INCORRECTLY FORMATTED COURSE STRING: '%s'\n", line);
            ++invalid_courses;
        }
        while (getchar() != '\n')
            ;
    }
    printf("=== %d COURSES HAVE BEEN ADDED ===\n", courses_added);
    if (invalid_courses > 0)
        printf("*** ENCOUNTERED %d INCORRECTLY FORMED LINES ***\n\n", invalid_courses);
}

// Initialize Student Registrations
// Reads one registration per line (format: <student name>, <course number>).
// Empty line ends the query
void initialize_student_regns()
{
    printf("ENTER THE STUDENTS LIST:\n");
    char line[64] = {'S', 0}; // initializing to a non-empty string
    int course;
    char student[64];
    int students_registered = 0, invalid_lines = 0;

    while (scanf("%[^\n]", &line) > 0)
    {
        printf("Line: %s\n", line);

        if(2 == sscanf(line, "%[^,], %d", student, &course))
        {
            printf("Registering '%s' for the course with id %d\n", student, course);
            ++students_registered;
        }
        else
        {
            printf("*** INCORRECTLY FORMATTED COURSE STRING: '%s'\n", line);
            ++invalid_lines;
        }
        while (getchar() != '\n')
            ;
    }
    printf("=== %d STUDENTS HAVE BEEN REGISTERED SUCCESSFULLY ===\n", students_registered);
    if (invalid_lines > 0)
        printf("*** ENCOUNTERED %d INCORRECTLY FORMED LINES ***\n\n", invalid_lines);
}

void process_query(char *query)
{
    printf("YOUR QUERY IS: %s\n", query);
    int course_num1, course_num2;
    int number;
    float gpa;
    char grade[4];
    char student_name[128];
    
    if (1 == sscanf(query, "GET ALL STUDENTS REGISTERED FOR THE COURSE %d", &course_num1))
        students_for_course(course_num1);
    else if(1 == sscanf(query, "GET ALL COURSES WHICH HAVE MORE THAN %d STUDENTS", &number))
        courses_n_students(number);
    else if(1 == sscanf(query, "GET ALL COURSES %s HAS REGISTERED FOR", student_name))
        courses_for_student(student_name);
    else if(2 == sscanf(query, "GET ALL STUDENTS REGISTERED FOR BOTH THE COURSES %d AND %d", &course_num1, &course_num2))
        students_for_both_courses(course_num1, course_num2);
    else if(1 == sscanf(query, "GET NUMBER OF STUDENTS REGISTERED FOR %d", &course_num1))
        num_stud_for_course(course_num1);
    else if(2 == sscanf(query, "REGISTER STUDENT %s FOR THE COURSE %d", &student_name, &course_num1))
        register_for_course(student_name, course_num1);
    else if(2 == sscanf(query, "UNREGISTER STUDENT %s FOR THE COURSE %d", &student_name, &course_num1))
        unregister_for_course(student_name, course_num1);
    else if(2 == sscanf(query, "HAS STUDENT %s REGISTERED FOR THE COURSE %d ?", &student_name, &course_num1))
        is_stud_in_course(student_name, course_num1);
    else if(1 == sscanf(query, "GET ALL STUDENTS WHO HAVE REGISTERED FOR MORE THAN %d COURSES", &number))
        students_n_courses(number);
    else if(0 == strcmp(query, "GET A LIST OF ALL STUDENTS"))
        print_students();
    else if(0 == strcmp(query, "GET A LIST OF ALL COURSES"))
        print_courses();
    else if(2 == sscanf(query, "GET ALL %d CREDIT COURSES %s HAS REGISTERED FOR", &number, student_name))
        n_cred_courses_for_stud(number, student_name);
    else if(1 == sscanf(query, "GET ALL %d CREDIT COURSES", &number))
        n_cred_courses(number);
    else if(1 == sscanf(query, "GET THE TOTAL NUMBER OF CREDITS %s REGISTERED FOR", student_name))
        credits_student(student_name);
    else if(1 == sscanf(query, "GET ALL STUDENTS WHO REGISTERED FOR AT LEAST %d CREDITS", &number))
        at_least_n_creds(number);
    else if(1 == sscanf(query, "GET A LIST OF GRADES AND THE GPA OF STUDENT %s", student_name))
        student_grades(student_name);
    else if(1 == sscanf(query, "GET A LIST OF ALL STUDENTS WHO HAS A GPA BELOW %f", &gpa))
        students_below_gpa(gpa);
    else if(1 == sscanf(query, "GET A LIST OF ALL STUDENTS WHO HAS A GPA ABOVE %f", &gpa))
        students_above_gpa(gpa);
    else if(2 == sscanf(query, "GET A LIST OF ALL STUDENTS WHOSE GRADE IS %s AND ABOVE IN %d", grade, &course_num1))
        students_passing_course(grade, course_num1);
    else if(2 == sscanf(query, "GET A LIST OF ALL STUDENTS WHOSE GRADE IS BELOW %s IN %d", grade, &course_num1))
        students_failing_course(grade, course_num1);
    else if(3 == sscanf(query, "ASSIGN GRADE %s TO STUDENT %s FOR THE COURSE %d", grade, student_name, &course_num1))
        assign_grade(grade, student_name, course_num1);
    else if(0 == strcmp(query, "INITIALIZE COURSES"))
        initialize_courses();
    else if(0 == strcmp(query, "INITIALIZE STUDENT REGISTRATIONS"))
        initialize_student_regns();
    else
        printf("*** BAD QUERY: '%s' ***\n", query);
    return;
}

int main()
{
    char response = 'y';
    char query[1024];
    add_course("Real Analysis", 204, 4);
    add_course("Introductory Course", 101, 4);
    add_course("Algebraic Combinatorics", 319, 3);
    add_student("Pranav", 204);
    add_student("Yagami", 319);

    while (response == 'y')
    {
        printf("ENTER YOUR QUERY:\n");
        scanf("%[^\n]", &query);
        while (getchar() != '\n')
            ;
        process_query(query);
        printf("CONTINUE(y/n)? ");
        scanf("%c", &response);
        while (getchar() != '\n')
            ;
    }
    free_courses();
}