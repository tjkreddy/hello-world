/**
 * @file student.h
 * @brief Header file containing the Student class definition
 * @author T Jugal Kishore
 * @date January 29, 2025
 * 
 * This file contains the definition of the Student class which represents
 * a student in the university management system. It handles student data
 * including personal information, academic records, and course enrollment.
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

/**
 * @brief Represents the academic performance level of a student
 * 
 * This enum class defines different academic standing levels that a student
 * can have based on their CGPA.
 */
enum class AcademicStanding {
    EXCELLENT,    /**< CGPA >= 9.0 */
    GOOD,        /**< CGPA >= 7.0 and < 9.0 */
    SATISFACTORY,/**< CGPA >= 5.0 and < 7.0 */
    PROBATION    /**< CGPA < 5.0 */
};

/**
 * @brief Class representing a student in the university
 * 
 * The Student class maintains information about a student including their
 * personal details, enrolled courses, and academic performance. It provides
 * methods to manage this information and calculate various academic metrics.
 */
class Student {
private:
    std::string studentId;     /**< Unique identifier for the student */
    std::string name;          /**< Full name of the student */
    std::string department;    /**< Department the student belongs to */
    float cgpa;               /**< Current CGPA of the student */
    int semester;             /**< Current semester of the student */
    std::vector<std::string> enrolledCourses; /**< List of courses currently enrolled */

public:
    /**
     * @brief Default constructor
     * 
     * Creates a new Student object with default values.
     */
    Student();

    /**
     * @brief Parameterized constructor
     * 
     * @param id Student's unique identifier
     * @param n Student's full name
     * @param dept Student's department
     * @pre id should be a valid student ID format
     * @throws std::invalid_argument if id format is invalid
     */
    Student(const std::string& id, const std::string& n, const std::string& dept);

    /**
     * @brief Enrolls the student in a new course
     * 
     * @param courseCode The code of the course to enroll in
     * @return true if enrollment was successful
     * @return false if enrollment failed (e.g., course already enrolled)
     * @throws std::runtime_error if maximum course limit reached
     */
    bool enrollInCourse(const std::string& courseCode);

    /**
     * @brief Updates the student's CGPA
     * 
     * @param newCGPA New CGPA value
     * @pre newCGPA must be between 0.0 and 10.0
     * @post Student's CGPA is updated if value is valid
     * @throws std::out_of_range if CGPA is not in valid range
     */
    void updateCGPA(float newCGPA);

    /**
     * @brief Gets the student's current academic standing
     * 
     * @return AcademicStanding enum value representing current standing
     */
    AcademicStanding getAcademicStanding() const;

    /**
     * @brief Gets the list of enrolled courses
     * 
     * @return std::vector<std::string> List of course codes
     */
    std::vector<std::string> getEnrolledCourses() const;

    /**
     * @brief Advances the student to next semester
     * 
     * @return true if advancement was successful
     * @return false if advancement failed (e.g., academic probation)
     */
    bool advanceToNextSemester();

    // Getters and setters
    std::string getStudentId() const { return studentId; }
    std::string getName() const { return name; }
    std::string getDepartment() const { return department; }
    float getCGPA() const { return cgpa; }
    int getSemester() const { return semester; }
};

#endif // STUDENT_H
