# Database Client of Academic System
_November 2015_  
_By Tian Zhang and Di Tian_   
_Course: Intorduction to Database Systems, Northwestern University, Evanston, IL_

Develop a database client along the lines of Northwestern Caesar’s academic system. 
Use C API to connect to the MySQL database and stored procedures to implement the functionality as below.
![functionality](https://github.com/zhtiansweet/Database-Client-of-Academic-System/blob/master/functionality.png)

## Login
The system first asks the students to login using their username and password. Those two input arguments must be matched against 
the database. When successfully logged in, students would be directed to the Student Menu screen. If username or password is incorrect, 
an error message would be given. 

## Student Menu
The Student Menu lists the student’s current courses of this quarter and year (use clock to programmatically determine the current year
and quarter; the courses currently being taken are also in the Transcript but the grades are NULL), plus the following options: Transcript, Enroll,
Withdraw, Personal Details and Logout. Logout would log out the student and go back to the login page.

## Transcript
The TRANSCRIPT screen lists logged-in students their full transcript with all courses and grades. The student would be provided with 
an option to see details of any of the courses listed in the transcript or go back to the Student Menu. If the student wants to see 
details of any course, then he/she would be asked to enter the course number and the details of the corresponding course would be
shown. The course details include: the course number and title, the year and quarter when the student took the course, 
the number of enrolled students, the maximum enrollment and the lecturer (name), the grade scored by the student.

## Enroll
The Enroll screen allows logged-in students to enroll in a new course. Students are able to select a specific course offering 
in this screen, but only subject offerings of the current year and quarter or the following quarter would be presented. 
Students can only enroll in courses whose direct pre-requisites they have passed (not failed or incomplete) and whose maximum 
enrollment number has not been reached (i.e. MaxEnrollment > Enrollment). On successful enrollment, a new entry in the Transcript 
table would be created with a NULL grade, plus the Enrollment attribute of the corresponding course would be increased by one. 
In case the student cannot enroll because he/she has not cleared the prerequisites, those prerequisites would be printed on the screen. 

## Withdraw
The Withdraw screen allows logged-in students to withdraw from a course that they are currently enrolled in.Students can only withdraw 
from a unit that they have not finished so far (i.e. grade is NULL). If successful, the corresponding Transcript entry would be 
removed and the current Enrollment number of the corresponding course shall be decreased by one.   

If the Enrollment number goes below 50% of the MaxEnrollment, then a warning message would be shown on the screen.

## Personal Details
The Personal Details screen shows the current personal record of the student and allows him/her to change his/her password and address. 
On submission, the corresponding Student record would be updated accordingly. Students cannot change their student id or name.
