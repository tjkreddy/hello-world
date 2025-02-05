/**
 * @file course_registration.cpp
 * @brief Implementation of the course registration system
 * @author tjkreddy
 * @date Feb 5, 2025
 * 
 * @details This file implements a course registration system that manages:
 * - Course enrollment and withdrawal
 * - Course capacity management
 * - Prerequisites verification
 * - Registration period enforcement
 * 
 * The system ensures proper course allocation while maintaining academic integrity
 * and administrative requirements.
 */

#include <map>
#include <set>
#include <stdexcept>
#include <ctime>
#include "student.h"

/**
 * @brief Represents the registration status for a course
 * 
 * Used to track various states a course registration can be in,
 * from initial registration attempt to final confirmation.
 */
enum class RegistrationStatus {
    SUCCESS,            /**< Registration completed successfully */
    COURSE_FULL,       /**< Course has reached maximum capacity */
    PREREQ_NOT_MET,    /**< Prerequisites not satisfied */
    TIME_CONFLICT,     /**< Schedule conflicts with another course */
    ALREADY_ENROLLED,  /**< Student already enrolled in course */
    REGISTRATION_CLOSED /**< Registration period has ended */
};

/**
 * @brief Class managing course registration operations
 * 
 * CourseRegistration handles all aspects of course enrollment including:
 * - Validating registration requirements
 * - Managing course capacities
 * - Handling registration periods
 * - Tracking enrolled students
 */
class CourseRegistration {
private:
    /** @brief Structure to hold course information */
    struct CourseInfo {
        std::string courseName;        /**< Name of the course */
        int maxCapacity;              /**< Maximum number of students allowed */
        std::set<std::string> prerequisites; /**< List of prerequisite courses */
        std::set<std::string> enrolledStudents; /**< Currently enrolled students */
        time_t registrationDeadline;  /**< Deadline for course registration */
    };

    std::map<std::string, CourseInfo> courses; /**< Database of all courses */
    
    /**
     * @brief Validates if a student meets course prerequisites
     * 
     * @param student Reference to the student object
     * @param courseCode Code of the course to validate
     * @return true if prerequisites are met
     * @return false if any prerequisite is missing
     */
    bool validatePrerequisites(const Student& student, const std::string& courseCode) const;

public:
    /**
     * @brief Adds a new course to the registration system
     * 
     * @param courseCode Unique identifier for the course
     * @param courseName Name of the course
     * @param capacity Maximum number of students allowed
     * @param prerequisites List of prerequisite course codes
     * @param deadline Registration deadline for the course
     * 
     * @throws std::invalid_argument if course code already exists
     * @throws std::out_of_range if capacity is negative
     * 
     * Example usage:
     * @code
     * CourseRegistration reg;
     * std::set<std::string> prereqs = {"CS101", "MATH201"};
     * reg.addCourse("CS201", "Data Structures", 60, prereqs, time(nullptr) + 86400);
     * @endcode
     */
    void addCourse(const std::string& courseCode, 
                  const std::string& courseName,
                  int capacity,
                  const std::set<std::string>& prerequisites,
                  time_t deadline);

    /**
     * @brief Registers a student for a course
     * 
     * @param student Student attempting to register
     * @param courseCode Code of the course to register for
     * @return RegistrationStatus indicating the result of registration attempt
     * 
     * @note This method performs several checks:
     * - Verifies course exists
     * - Checks course capacity
     * - Validates prerequisites
     * - Confirms registration deadline
     * 
     * @warning Registration after the deadline will be automatically rejected
     */
    RegistrationStatus registerStudent(Student& student, const std::string& courseCode);

    /**
     * @brief Withdraws a student from a course
     * 
     * @param studentId ID of the student to withdraw
     * @param courseCode Code of the course to withdraw from
     * @return true if withdrawal was successful
     * @return false if student wasn't enrolled or course doesn't exist
     */
    bool withdrawStudent(const std::string& studentId, const std::string& courseCode);

    /**
     * @brief Gets current enrollment count for a course
     * 
     * @param courseCode Code of the course to check
     * @return int Number of enrolled students
     * @throws std::out_of_range if course doesn't exist
     */
    int getEnrollmentCount(const std::string& courseCode) const;

    /**
     * @brief Checks if a course is full
     * 
     * @param courseCode Code of the course to check
     * @return true if course has reached maximum capacity
     * @return false if there are still seats available
     * @throws std::out_of_range if course doesn't exist
     */
    bool isCourseFull(const std::string& courseCode) const;
};

// Implementation of CourseRegistration methods

void CourseRegistration::addCourse(const std::string& courseCode,
                                 const std::string& courseName,
                                 int capacity,
                                 const std::set<std::string>& prerequisites,
                                 time_t deadline) {
    if (courses.find(courseCode) != courses.end()) {
        throw std::invalid_argument("Course already exists");
    }
    if (capacity < 0) {
        throw std::out_of_range("Capacity must be non-negative");
    }
    
    CourseInfo info{
        courseName,
        capacity,
        prerequisites,
        std::set<std::string>(),
        deadline
    };
    courses[courseCode] = info;
}

RegistrationStatus CourseRegistration::registerStudent(Student& student, 
                                                     const std::string& courseCode) {
    auto courseIt = courses.find(courseCode);
    if (courseIt == courses.end()) {
        throw std::invalid_argument("Course does not exist");
    }

    CourseInfo& course = courseIt->second;

    // Check registration deadline
    if (time(nullptr) > course.registrationDeadline) {
        return RegistrationStatus::REGISTRATION_CLOSED;
    }

    // Check if already enrolled
    if (course.enrolledStudents.find(student.getStudentId()) != 
        course.enrolledStudents.end()) {
        return RegistrationStatus::ALREADY_ENROLLED;
    }

    // Check course capacity
    if (course.enrolledStudents.size() >= course.maxCapacity) {
        return RegistrationStatus::COURSE_FULL;
    }

    // Check prerequisites
    if (!validatePrerequisites(student, courseCode)) {
        return RegistrationStatus::PREREQ_NOT_MET;
    }

    // Register student
    course.enrolledStudents.insert(student.getStudentId());
    student.enrollInCourse(courseCode);
    return RegistrationStatus::SUCCESS;
}

bool CourseRegistration::validatePrerequisites(const Student& student, 
                                             const std::string& courseCode) const {
    const auto& courseIt = courses.find(courseCode);
    if (courseIt == courses.end()) {
        return false;
    }

    const auto& prereqs = courseIt->second.prerequisites;
    const auto& completedCourses = student.getEnrolledCourses();

    for (const auto& prereq : prereqs) {
        if (std::find(completedCourses.begin(), completedCourses.end(), prereq) 
            == completedCourses.end()) {
            return false;
        }
    }
    return true;
}

bool CourseRegistration::withdrawStudent(const std::string& studentId, 
                                       const std::string& courseCode) {
    auto courseIt = courses.find(courseCode);
    if (courseIt == courses.end()) {
        return false;
    }

    return courseIt->second.enrolledStudents.erase(studentId) > 0;
}

int CourseRegistration::getEnrollmentCount(const std::string& courseCode) const {
    auto courseIt = courses.find(courseCode);
    if (courseIt == courses.end()) {
        throw std::out_of_range("Course does not exist");
    }
    return courseIt->second.enrolledStudents.size();
}

bool CourseRegistration::isCourseFull(const std::string& courseCode) const {
    auto courseIt = courses.find(courseCode);
    if (courseIt == courses.end()) {
        throw std::out_of_range("Course does not exist");
    }
    return courseIt->second.enrolledStudents.size() >= courseIt->second.maxCapacity;
}
