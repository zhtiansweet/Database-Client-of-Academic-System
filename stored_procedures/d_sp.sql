DELIMITER //

DROP PROCEDURE IF EXISTS candidate_course; //
CREATE PROCEDURE candidate_course (IN id INT(11),
								   IN cur_q_year INT(11),
                                   IN cur_q_name CHAR(2),
                                   IN next_q_year INT(11),
                                   IN next_q_name CHAR(2))
BEGIN
	SELECT UoSCode, UoSName, Year, Semester
	FROM uosoffering JOIN unitofstudy USING (UoSCode)
    WHERE ((Year = cur_q_year AND Semester = cur_q_name) OR
    	  (Year = next_q_year AND Semester = next_q_name)) AND
          Enrollment < MaxEnrollment AND
          UoSCode NOT IN
          (SELECT UoSCode
           FROM student join transcript ON student.Id = transcript.StudId
           WHERE Year = cur_q_year AND Semester = cur_q_name AND Id = id
           AND (Grade IS NULL OR (Grade <> "F" AND Grade <> "NC")))
    ORDER BY uosoffering.UoSCode;
END //

DELIMITER ;