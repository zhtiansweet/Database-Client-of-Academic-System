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
          (UoSCode, Year, Semester) NOT IN
          (SELECT UoSCode, Year, Semester
           FROM transcript
           WHERE StudId = id AND ((Year = cur_q_year AND Semester = cur_q_name) OR
    	  (Year = next_q_year AND Semester = next_q_name)))
    ORDER BY uosoffering.UoSCode;
END //


DROP PROCEDURE IF EXISTS enroll; //
CREATE PROCEDURE enroll (IN id INT(11),
						 IN uos_code CHAR(8),
						 IN uos_q_year INT(11),
						 IN uos_q_name CHAR(2),
						 IN uos_begin DATE)

BEGIN
	IF	EXISTS (SELECT PrereqUoSCode, UoSName FROM requires
			    INNER JOIN unitofstudy ON PrereqUoSCode = unitofstudy.UoSCode
			    WHERE requires.UoSCode = uos_code AND PrereqUoSCode NOT IN (SELECT UoSCode FROM transcript WHERE StudId = id)
			    AND EnforcedSince < uos_begin)
		OR
		EXISTS (SELECT PrereqUoSCode, UoSName FROM requires
			   LEFT JOIN transcript ON requires.PrereqUoSCode = transcript.UoSCode
			   LEFT JOIN unitofstudy ON requires.PrereqUoSCode = unitofstudy.UoSCode
			   WHERE (requires.UoSCode = uos_code AND studid = id AND EnforcedSince < uos_begin AND (Grade IS NULL OR Grade = "F")))
		THEN
			SELECT PrereqUoSCode, UoSName FROM requires
			INNER JOIN unitofstudy ON PrereqUoSCode = unitofstudy.UoSCode
			WHERE requires.UoSCode = uos_code AND PrereqUoSCode NOT IN (SELECT UoSCode FROM transcript WHERE StudId = id)
			AND EnforcedSince < uos_begin

			UNION

			SELECT PrereqUoSCode, UoSName FROM requires
			LEFT JOIN transcript ON requires.PrereqUoSCode = transcript.UoSCode
			LEFT JOIN unitofstudy ON requires.PrereqUoSCode = unitofstudy.UoSCode
			WHERE (requires.UoSCode = uos_code AND studid = id AND EnforcedSince < uos_begin AND (Grade IS NULL OR Grade = "F"));

	ELSE
		INSERT INTO transcript VALUES(id, uos_code, uos_q_name, uos_q_year, NULL);
		UPDATE uosoffering SET Enrollment = Enrollment + 1
		WHERE UoSCode = uos_code AND Semester = uos_q_name AND Year = uos_q_year;
		SELECT * FROM uosoffering WHERE Enrollment = -1;
	END IF;
END //

DELIMITER ;