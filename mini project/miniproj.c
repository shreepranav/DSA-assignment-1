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
    struct Course *next;
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

Course *courses_offered = NULL;
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

// Returns length of the '\0' terminated string 's'
int stringlen(const char* s)
{
    int i = 0;
    if (s != NULL)
        while (s[i] != '\0')
            ++i;
    return i;
}

// Copies string 'src' into 'dest' upto and including '\0'.
// Assumes that 'dest' has enough memory already allocated.
void stringcopy(char* dest, const char* src)
{
    while(*(dest++) = *(src++))
        ;
}

// Copies the string in src into a newly heap allocated string and returns it.
char *strcopy(const char *src)
{
    char *dest = (char *) malloc(stringlen(src) + 1);
    stringcopy(dest, src);
    return dest;
}

// Appends s2, s3 and s4 (separated by ' ') to s1 and returns s1 back.
// Assumes that s1 has enough memory to hold all the strings.
// If any of the strings s2, s3 or s4 is empty or NULL, then that string and all
// subsequent strings are not appended to s1
char *append_strings(char *s1, const char *s2, const char *s3, const char *s4)
{
    int l1 = stringlen(s1);
    int l2 = stringlen(s2);
    int l3 = stringlen(s3);
    int l4 = stringlen(s4);

    if (l2 == 0)
        return s1;
    s1[l1] = ' ';
    stringcopy(s1 + l1 + 1, s2);
    if (l3 == 0)
        return s1;
    s1[l1 + l2 + 1] = ' ';
    stringcopy(s1 + l1 + l2 + 2, s3);
    if (l4 == 0)
        return s1;
    s1[l1 + l2 + l3 + 2] = ' ';
    stringcopy(s1 + l1 + l2 + l3 + 3, s4);
    return s1;
}

void add_course(char *name, int code, int credits)
{
    if (courselist_len == MAX_COURSES)
    {
        printf("Too many courses!\n");
        return;
    }
    Course *new_course = (Course *)malloc(sizeof(Course));
    new_course->course_name = strcopy(name);
    new_course->course_code = code;
    new_course->credits = credits;
    Course *p = courses_offered;
    if (p == NULL || p->credits <= credits)
    {
        courses_offered = new_course;
        new_course->next = p; 
        return;
    }
    while(p->next && p->next->credits > credits)
        p = p->next;

    new_course->next = p->next;
    p->next = new_course;
}

void free_courses()
{
    Course *p = courses_offered;
    while (p)
    {
        free(p->course_name);
        Course *temp = p->next;
        free(p);
        p = temp;
    }
}

Course *find_course(int code)
{
    for (Course *p = courses_offered; p; p = p->next)
        if (p->course_code == code)
            return p;
    return NULL;
}

Student *find_student(char *name)
{
    for (int i = 0; i < studlist_len; i++)
        if (strcmp(name, students[i]->name) == 0)
            return students[i];
    return NULL;
}

CoursePerf *add_course_to_student(CoursePerf **h, int course_code)
{
    Course* course_to_add = find_course(course_code);
    if (course_to_add == NULL)
    {
        printf("ERROR: Course does not exist!\n");
        return NULL;
    }
    int credits = course_to_add->credits;
    CoursePerf *new_node = (CoursePerf *)malloc(sizeof(CoursePerf));
    new_node->course = course_to_add;
    new_node->grade = 11;
    CoursePerf *p = *h;
    if (*h == NULL || (*h)->course->credits <= credits)
    {
        new_node->next = *h;
        *h = new_node;
        return new_node;
    }
    while (p->next && p->next->course->credits > credits)
        p = p->next;

    new_node->next = p->next;
    p->next = new_node;
    return new_node;
}

int delete_course_from_student(CoursePerf **h, int course_code)
{
    CoursePerf *p = *h;
    if (p->course->course_code == course_code)
    {
        *h = p->next;
        free(p);
        return 0;    
    }

    while(p->next)
    {
        if(p->next->course->course_code == course_code)
        {
            CoursePerf* temp = p->next;
            p->next = temp->next;
            free(temp);
            return 0;
        }
        p = p->next;
    }
    return 1;
}

void add_student(char *name, int course_code)
{
    Student *s = find_student(name);
    if (s == NULL)
    {
        s = (Student *)malloc(sizeof(Student));
        s->name = strcopy(name);
        s->num_courses = 0;
        s->course_list = NULL;
        students[studlist_len++] = s;
    }
    if (s->num_courses >= MAX_COURSES_PER_STUDENT)
        printf("ERROR: %s has already registered for max courses!\n", name);
    else
    {
        add_course_to_student(&(s->course_list), course_code);
        s->num_courses++;
    }
}

void free_students()
{
    for (int i = 0; i < studlist_len; i++)
    {
        CoursePerf *temp = NULL;
        for (CoursePerf *p = students[i]->course_list; p; p = temp)
        {
            temp = p->next;
            free(p);
        }
        free(students[i]->name);
        free(students[i]);
    }
    studlist_len = 0;
}

const char *GRADES[] = {"F", "D", "C", "B", "B+", "A", "A+", "U"};

const char *num_to_grade(int g)
{
    if (g < 5 && g != 0 || g > sizeof(GRADES) / sizeof(const char *) + 3)
        return "INVLAID_GRADE";
    return GRADES[g == 0 ? 0 : g - 4];
}

// Returns the grade point value for the string grade.
// U corresponds to 11, and an invalid grade corresponds to -1
int grade_to_num(char *grade)
{
    int result = -1;
    for (int g = 0; g < sizeof(GRADES) / sizeof(const char *); ++g)
        if (strcmp(GRADES[g], grade) == 0)
            result = (g == 0) ? 0 : g + 4;
    return result;
}

CoursePerf* student_courseperf(Student *s, Course* c)
{
    for (CoursePerf *p = s->course_list; p ; p = p->next)
        if (p->course == c)
            return p;
    return NULL;
}

int course_strength(Course *course)
{
    int res = 0;
    for (int i = 0; i < studlist_len; i++)
        if (student_courseperf(students[i], course))
            ++res;
    return res;
}

float gpa(Student *s)
{
    float gradepoints = 0.0;
    int credits = 0;
    for (CoursePerf *p = s->course_list; p; p = p->next)
    {
        if (p->grade < 11)
        {
            credits += p->course->credits;
            gradepoints += p->course->credits * p->grade;
        }
    }
    int n = (int) 10 * (gradepoints / credits + 0.05);
    return n / 10.0;
}

void print_course(Course *course)
{
    if (course == NULL)
        return;
    printf("%d, %s, %d\n", course->course_code, course->course_name, course->credits);
}

// Query 1: GET ALL STUDENTS REGISTERED FOR THE COURSE <course number>
void students_for_course(int course_number)
{
    printf("Here's all the students for course %d:\n", course_number);
    Course* c = find_course(course_number);
    if (c == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_number);
        return;
    }

    for (int i = 0; i < studlist_len; i++)
        if (student_courseperf(students[i], c))
            printf("%s\n", students[i]->name);
}

// Query 2: GET ALL COURSES <student name> HAS REGISTERED FOR
void courses_for_student(char *student_name)
{
    printf("Here's all the courses %s has registered for:\n", student_name);
    Student *s = find_student(student_name);
    if (s == NULL)
    {
        printf("ERROR: Student %s does not exist!\n", student_name);
        return;
    }
    for (CoursePerf *p = s->course_list; p; p = p->next)
        print_course(p->course);
}

// Query 3: GET ALL STUDENTS REGISTERED FOR BOTH THE COURSES <course number> AND <course number>
void students_for_both_courses(int course_num1, int course_num2)
{
    printf("Here's all the students that registered for both %d and %d:\n", course_num1, course_num2);
    Course* c1 = find_course(course_num1);
    Course* c2 = find_course(course_num2);
    if (c1 == NULL || c2 == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", (c1 == NULL) ? course_num1 : course_num2);
        return;
    }

    for (int i = 0; i < studlist_len; i++)
        if(student_courseperf(students[i], c1) && student_courseperf(students[i], c2))
            printf("%s\n", students[i]->name);
}

// Query 4: GET NUMBER OF STUDENTS REGISTERED FOR <course number>
void num_stud_for_course(int course_num)
{
    printf("Here's number of students in course %d:\n", course_num);
    Course* c = find_course(course_num);
    if (c == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_num);
        return;
    }

    printf("%d\n", course_strength(c));
}

// Query 5: REGISTER STUDENT <student name> FOR THE COURSE <course number>
void register_for_course(char *student_name, int course_num)
{
    if (find_course(course_num) == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_num);
        return;
    }
    printf("Registering %s for course %d\n", student_name, course_num);
    /*
    Student *s = find_student(student_name);
    if (s == NULL)
    {
        printf("ERROR: Student %s doesn't exist!", student_name);
        return;
    }

    if (s->num_courses == MAX_COURSES_PER_STUDENT)
    {
        printf("ERROR: Student has already registered for max courses!");
        return;
    }
    */
    add_student(student_name, course_num);
}

// Query 6: UNREGISTER STUDENT <student name> FOR THE COURSE <course number>
void unregister_for_course(char *student, int course_num)
{
    Student *s = find_student(student);
    if (s == NULL)
    {
        printf("ERROR: Student %s does not exist!\n", student);
        return;
    }

    int res = delete_course_from_student(&(s->course_list), course_num);
    if (res == 1)
    {
        printf("ERROR: Student %s is not registered for course %d\n", student, course_num);
        return;
    }
    s->num_courses--;
    printf("Unregistered %s for course %d\n", student, course_num);
}

// Query 7: HAS STUDENT <student name> REGISTERED FOR THE COURSE <course number> ?
void is_stud_in_course(char *student, int course_num)
{
    Student *s = find_student(student);
    if (s == NULL)
    {
        printf("ERROR: Student %s does not exist!\n", student);
        return;
    }
    Course* c = find_course(course_num);
    if (c == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_num);
        return;
    }

    if (student_courseperf(s, c))
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
    for (int i = 0; i < studlist_len; i++)
        if (students[i]->num_courses > num_courses)
            printf("%s\n", students[i]->name);
}

// Query 9: GET A LIST OF ALL STUDENTS
void print_students()
{
    printf("Here is a list of all students:\n");
    for (int i = 0; i < studlist_len; i++)
        printf("%s\n", students[i]->name);
}

// Query 10: GET A LIST OF ALL COURSES
void print_courses()
{
    printf("Here is a list of all courses:\n");
    for (Course *p = courses_offered; p; p = p->next)
        print_course(p);
}

// Query 11: GET ALL COURSES WHICH HAVE MORE THAN <number> STUDENTS
void courses_n_students(int strength)
{
    if (strength > MAX_COURSE_STRENGTH)
    {
        printf("More than %d students are not registered for any course!\n", MAX_COURSE_STRENGTH);
        return;
    }
    
    printf("Here's all courses with at least %d students:\n", strength);
    for (Course *p = courses_offered; p; p = p->next)
        if (course_strength(p) > strength)
            print_course(p);
}

// Query 12: GET ALL <number> CREDIT COURSES <student name> HAS REGISTERED FOR
void n_cred_courses_for_stud(int number, char *student)
{
    if (number < 1 || number > 5)
    {
        printf("ERROR: Invalid number of credits for a course: %d\n", number);
        return;
    }
    Student *s = find_student(student);
    if (s == NULL)
    {
        printf("ERROR: Student %s does not exist!\n", student);
        return;
    }
    
    printf("Here's all the %d credit courses %s has registered for:\n", number, student);
    for (CoursePerf *p = s->course_list; p; p = p->next)
        if (p->course->credits == number)
            print_course(p->course);
}

// Query 13: GET ALL <number> CREDIT COURSES
void n_cred_courses(int number)
{
    if (number < 1 || number > 5)
    {
        printf("ERROR: Invalid number of credits for a course: %d\n", number);
        return;
    }

    printf("Here's all %d credit courses:\n", number);
    for (Course *p = courses_offered; p; p = p->next)
        if (p->credits == number)
            print_course(p);
}

// Query 14: GET THE TOTAL NUMBER OF CREDITS <student name> REGISTERED FOR
void credits_student(char *student)
{
    Student *s = find_student(student);
    if (s == NULL)
    {
        printf("ERROR: Student %s does not exist.\n", student);
        return;
    }
    int credits = 0;
    for (CoursePerf *p = s->course_list; p != NULL; p = p->next)
        credits += p->course->credits;
    printf("Here's the total number of credits '%s' has taken:\n", student);
    printf("%d\n", credits);
}

// Query 15: GET ALL STUDENTS WHO REGISTERED FOR AT LEAST <number> CREDITS
void at_least_n_creds(int number)
{
    if (number > 30)
    {
        printf("No student can register for more than 30 credits!\n");
        return;
    }

    printf("Here's the list of all students taking at least %d credits:\n", number);
    for (int i = 0; i < studlist_len; i++)
    {
        int credits = 0;
        for (CoursePerf *p = students[i]->course_list; p != NULL; p = p->next)
            credits += p->course->credits; 
        if (credits >= number)
            printf("%s\n", students[i]->name);   
    }
}

// Query 16: GET A LIST OF GRADES AND THE GPA OF STUDENT <student name>
void student_grades(char *student)
{
    Student *s = find_student(student);
    if (s == NULL)
    {
        printf("ERROR: Student %s does not exist!\n", student);
        return;
    }
    printf("Here's a list of grades and GPA of %s:\n", student);
    for (CoursePerf *p = s->course_list; p; p = p->next)
        printf("%d: %s\n", p->course->course_code, num_to_grade(p->grade));
    printf("GPA: %.1f\n", gpa(s));
}

// Query 17: GET A LIST OF ALL STUDENTS WHO HAS A GPA BELOW <number>
void students_below_gpa(float grade)
{
    if (grade < 0 || grade > 10)
    {
        printf("ERROR: Invalid GPA: %g\n", grade);
        return;
    }
    printf("Here's a list of students with GPA below %g:\n", grade);
    for (int i = 0; i < studlist_len; i++)
        if (gpa(students[i]) < grade)
            printf("%s\n", students[i]->name);
}

// Query 18: GET A LIST OF ALL STUDENTS WHO HAS A GPA ABOVE <number>
void students_above_gpa(float grade)
{
    if (grade < 0 || grade > 10)
    {
        printf("ERROR: Invalid GPA: %g\n", grade);
        return;
    }

    printf("Here's a list of students with GPA above %g:\n", grade);
    for (int i = 0; i < studlist_len; i++)
        if (gpa(students[i]) >= grade)
            printf("%s\n", students[i]->name);
}

// Query 19: GET A LIST OF ALL STUDENTS WHOSE GRADE IS <grade> AND ABOVE IN <course number>
void students_above_grade(char *grade, int course_num)
{
    Course *course = find_course(course_num);
    if (course == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_num);
        return;
    }

    int g = grade_to_num(grade);

    printf("Here's a list of students with grade %s or above in course %d:\n", grade, course_num);
    for (int i = 0; i < studlist_len; i++)
    {
        CoursePerf *p = student_courseperf(students[i], course);
        if (p && p->grade < 11 && p->grade >= g)
            printf("%s\n", students[i]->name);
    }
}

// Query 20: GET A LIST OF ALL STUDENTS WHOSE GRADE IS BELOW <grade> IN <course number>
void students_below_grade(char *grade, int course_num)
{   
    Course *course = find_course(course_num);
    if (course == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_num);
        return;
    }

    int g = grade_to_num(grade);

    printf("Here's a list of students with grade below %s in course %d:\n", grade, course_num);
    for (int i = 0; i < studlist_len; i++)
    {
        CoursePerf *p = student_courseperf(students[i], course);
        if (p && p->grade < 11 && p->grade < g)
            printf("%s\n", students[i]->name);
    }
}

// Query 21: ASSIGN GRADE <grade> TO STUDENT <student name> FOR THE COURSE <course number>
void assign_grade(char *grade, char *student, int course_num)
{
    Course *c = find_course(course_num);
    if (c == NULL)
    {
        printf("ERROR: Course %d does not exist!\n", course_num);
        return;
    }
    printf("Assigned %s to %s in course %d\n", grade, student, course_num);
    Student *s = find_student(student);
    CoursePerf *p = student_courseperf(s, c);

    if (p == NULL)
        printf("ERROR: %s is not taking course %d\n", student, course_num);
    else
        p->grade = grade_to_num(grade);
}

// Query 22: ASSIGN GRADES FOR THE COURSE <course number>
void assign_grades(int course_num)
{
    char grade[4];
    Course *c = find_course(course_num);
    for (int i = 0; i < studlist_len; i++)
    {
        CoursePerf *p = student_courseperf(students[i], c);
        if (p == NULL)
            continue;
        printf("%s: ", students[i]->name);
        scanf("%s", grade);
        int g = grade_to_num(grade);
        if (g < 0)
        {
            printf("ERROR: Invalid Grade. Please try again.\n");
            i--;
        }
        else
            p->grade = g;
    }
}

// Initialize Courses
// Reads one course per line. (format: <course name>, <course title string>, <credits>)
// Empty line ends the query
void initialize_courses()
{
    printf("ENTER THE COURSE LIST:\n");
    char line[1024] = {'S', 0}; // initializing to a non-empty string
    int course_code;
    char title[1024];
    int credits;
    int courses_added = 0, invalid_courses = 0;

    while (scanf("%[^\n]", &line) > 0)
    {
        //printf("Line: %s\n", line);

        if (3 == sscanf(line, "%d, %[^,], %d", &course_code, title, &credits))
        {
            printf("Adding Course '%s' with id %d and %d credits\n", title, course_code, credits);
            add_course(title, course_code, credits);
            ++courses_added;
        }
        else
        {
            printf("ERROR: Incorrectly formatted input string: '%s'\n", line);
            ++invalid_courses;
        }
        while (getchar() != '\n')
            ;
    }
    //printf("=== %d COURSES HAVE BEEN ADDED ===\n", courses_added);
    //if (invalid_courses > 0)
    //   printf("*** ENCOUNTERED %d INCORRECTLY FORMED LINES ***\n\n", invalid_courses);
}

// Initialize Student Registrations
// Reads one registration per line (format: <student name>, <course number>).
// Empty line ends the query
void initialize_student_regns()
{
    printf("ENTER THE STUDENTS LIST:\n");
    char line[128] = {'S', 0}; // initializing to a non-empty string
    int course;
    char student[128];
    int students_registered = 0, invalid_lines = 0;

    while (scanf("%[^\n]", &line) > 0)
    {
        //printf("Line: %s\n", line);

        if (2 == sscanf(line, "%[^,], %d", student, &course))
        {
            if (find_course(course) == NULL)
                printf("ERROR: Course does not exist! Please try again.\n");
            else
            {
                printf("Registering '%s' for the course with id %d\n", student, course);
                add_student(student, course);
                ++students_registered;
            }
        }
        else
        {
            printf("ERROR: Incorrectly formatted input string: '%s'\n", line);
            ++invalid_lines;
        }
        while (getchar() != '\n')
            ;
    }
    //printf("=== %d STUDENTS HAVE BEEN REGISTERED SUCCESSFULLY ===\n", students_registered);
    //if (invalid_lines > 0)
    //    printf("*** ENCOUNTERED %d INCORRECTLY FORMED LINES ***\n\n", invalid_lines);
}

void process_query(char *query)
{
    printf("YOUR QUERY IS: %s\n", query);
    int course_num1, course_num2;
    int number;
    float gpa;
    char grade[4];
    char student[128] = {0}, s2[32] = {0}, s3[32] = {0}, s4[32] = {0};
    char lw[32] = {0}; // last word of the query

    if (1 == sscanf(query, "GET ALL STUDENTS REGISTERED FOR THE COURSE %d", &course_num1))
        students_for_course(course_num1);
    else if (2 == sscanf(query, "GET ALL COURSES WHICH HAVE MORE THAN %d %s", &number, lw) && strcmp("STUDENTS", lw) == 0)
        courses_n_students(number);
    else if (1 == sscanf(query, "GET ALL COURSES %s HAS REGISTERED %s", student, lw) && strcmp("FOR", lw) == 0 ||
             2 == sscanf(query, "GET ALL COURSES %s %s HAS REGISTERED %s", student, s2, lw) && strcmp("FOR", lw) == 0 ||
             3 == sscanf(query, "GET ALL COURSES %s %s %s HAS REGISTERED %s", student, s2, s3, lw) && strcmp("FOR", lw) == 0 ||
             4 == sscanf(query, "GET ALL COURSES %s %s %s %s HAS REGISTERED %s", student, s2, s3, s4, lw) && strcmp("FOR", lw) == 0)
        courses_for_student(append_strings(student, s2, s3, s4));
    else if (2 == sscanf(query, "GET ALL STUDENTS REGISTERED FOR BOTH THE COURSES %d AND %d", &course_num1, &course_num2))
        students_for_both_courses(course_num1, course_num2);
    else if (1 == sscanf(query, "GET NUMBER OF STUDENTS REGISTERED FOR %d", &course_num1))
        num_stud_for_course(course_num1);
    else if (2 == sscanf(query, "REGISTER STUDENT %s FOR THE COURSE %d", student, &course_num1) ||
             3 == sscanf(query, "REGISTER STUDENT %s %s FOR THE COURSE %d", student, s2, &course_num1) ||
             4 == sscanf(query, "REGISTER STUDENT %s %s %s FOR THE COURSE %d", student, s2, s3, &course_num1) ||
             5 == sscanf(query, "REGISTER STUDENT %s %s %s %s FOR THE COURSE %d", student, s2, s3, s4, &course_num1))
        register_for_course(append_strings(student, s2, s3, s4), course_num1);
    else if (2 == sscanf(query, "UNREGISTER STUDENT %s FOR THE COURSE %d", student, &course_num1) ||
             3 == sscanf(query, "UNREGISTER STUDENT %s %s FOR THE COURSE %d", student, s2, &course_num1) ||
             4 == sscanf(query, "UNREGISTER STUDENT %s %s %s FOR THE COURSE %d", student, s2, s3, &course_num1) ||
             5 == sscanf(query, "UNREGISTER STUDENT %s %s %s %s FOR THE COURSE %d", student, s2, s3, s4, &course_num1))
        unregister_for_course(append_strings(student, s2, s3, s4), course_num1);
    else if (2 == sscanf(query, "HAS STUDENT %s REGISTERED FOR THE COURSE %d ?", student, &course_num1) ||
             3 == sscanf(query, "HAS STUDENT %s %s REGISTERED FOR THE COURSE %d ?", student, s2, &course_num1) ||
             4 == sscanf(query, "HAS STUDENT %s %s %s REGISTERED FOR THE COURSE %d ?", student, s2, s3, &course_num1) ||
             5 == sscanf(query, "HAS STUDENT %s %s %s %s REGISTERED FOR THE COURSE %d ?", student, s2, s3, s4, &course_num1))
        is_stud_in_course(append_strings(student, s2, s3, s4), course_num1);
    else if (1 == sscanf(query, "GET ALL STUDENTS WHO HAVE REGISTERED FOR MORE THAN %d COURSES", &number))
        students_n_courses(number);
    else if (0 == strcmp(query, "GET A LIST OF ALL STUDENTS"))
        print_students();
    else if (0 == strcmp(query, "GET A LIST OF ALL COURSES"))
        print_courses();
    else if (2 == sscanf(query, "GET ALL %d CREDIT COURSES %s HAS REGISTERED FOR", &number, student) ||
             3 == sscanf(query, "GET ALL %d CREDIT COURSES %s %s HAS REGISTERED FOR", &number, student, s2) ||
             4 == sscanf(query, "GET ALL %d CREDIT COURSES %s %s %s HAS REGISTERED FOR", &number, student, s2, s3) ||
             5 == sscanf(query, "GET ALL %d CREDIT COURSES %s %s %s %s HAS REGISTERED FOR", &number, student, s2, s3, s4))
        n_cred_courses_for_stud(number, append_strings(student, s2, s3, s4));
    else if (1 == sscanf(query, "GET ALL %d CREDIT COURSES", &number))
        n_cred_courses(number);
    else if (1 == sscanf(query, "GET THE TOTAL NUMBER OF CREDITS %s REGISTERED FOR", student))
        credits_student(student);
    else if (1 == sscanf(query, "GET ALL STUDENTS WHO REGISTERED FOR AT LEAST %d CREDITS", &number))
        at_least_n_creds(number);
    else if (4 == sscanf(query, "GET A LIST OF GRADES AND THE GPA OF STUDENT %s %s %s %s", student, s2, s3, s4) ||
             3 == sscanf(query, "GET A LIST OF GRADES AND THE GPA OF STUDENT %s %s %s", student, s2, s3) ||
             2 == sscanf(query, "GET A LIST OF GRADES AND THE GPA OF STUDENT %s %s", student, s2) ||
             1 == sscanf(query, "GET A LIST OF GRADES AND THE GPA OF STUDENT %s", student))
        student_grades(append_strings(student, s2, s3, s4));
    else if (1 == sscanf(query, "GET A LIST OF ALL STUDENTS WHO HAS A GPA BELOW %f", &gpa))
        students_below_gpa(gpa);
    else if (1 == sscanf(query, "GET A LIST OF ALL STUDENTS WHO HAS A GPA ABOVE %f", &gpa))
        students_above_gpa(gpa);
    else if (2 == sscanf(query, "GET A LIST OF ALL STUDENTS WHOSE GRADE IS %s AND ABOVE IN %d", grade, &course_num1))
        students_above_grade(grade, course_num1);
    else if (2 == sscanf(query, "GET A LIST OF ALL STUDENTS WHOSE GRADE IS BELOW %s IN %d", grade, &course_num1))
        students_below_grade(grade, course_num1);
    else if (3 == sscanf(query, "ASSIGN GRADE %s TO STUDENT %s FOR THE COURSE %d", grade, student, &course_num1) ||
             4 == sscanf(query, "ASSIGN GRADE %s TO STUDENT %s %s FOR THE COURSE %d", grade, student, s2, &course_num1) ||
             5 == sscanf(query, "ASSIGN GRADE %s TO STUDENT %s %s %s FOR THE COURSE %d", grade, student, s2, s3, &course_num1) ||
             6 == sscanf(query, "ASSIGN GRADE %s TO STUDENT %s %s %s %s FOR THE COURSE %d", grade, student, s2, s3, s4, &course_num1))
        assign_grade(grade, student, course_num1);
    else if (1 == sscanf(query, "ASSIGN GRADES FOR THE COURSE %d", &course_num1))
        assign_grades(course_num1);
    else if (0 == strcmp(query, "INITIALIZE COURSES"))
        initialize_courses();
    else if (0 == strcmp(query, "INITIALIZE STUDENT REGISTRATIONS"))
        initialize_student_regns();
    else
        printf("*** BAD QUERY: '%s' ***\n", query);
    return;
}

int main()
{
    char response = 'y';
    char query[1024];
    //add_course("Real Analysis", 204, 4);
    //add_course("Introductory Course", 101, 4);
    //add_course("Algebraic Combinatorics", 319, 3);
    //add_student("Pranav", 319);
    //add_student("Yagami", 204);

    /*
    stringcopy(query, "This is a long string");
    printf("%s\n", strconcat4("This is a long string.....", "", "", ""));
    printf("%s\n", strconcat4("Shreekar", "Varma", NULL, ""));
    printf("%s\n", strconcat4("E.", "Shreepranav", "Varma", NULL));
    printf("%s\n", strconcat4("Ding", "Dong.", "Ping", "Pong"));
    */

    while (response == 'y')
    {
        printf("ENTER YOUR QUERY:\n");
        scanf("%[^\n]", &query);
        while (getchar() != '\n')
            ;
        process_query(query);
        printf("CONTINUE(y/n)? ");
        do
        {
            scanf("%c", &response);
        } while (response == '\n');
        while (getchar() != '\n')
            ;
    }
    free_courses();
    free_students();
}