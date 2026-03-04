<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DSGN-1040: Digital Tools for Interaction - Module 2</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&family=JetBrains+Mono:wght@400;600&display=swap');
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
            line-height: 1.6;
            color: #2d3748;
            max-width: 8.5in;
            margin: 0 auto;
            padding: 0.75in;
            background: white;
        }
        
        .ascii-header {
            font-family: 'JetBrains Mono', 'Courier New', monospace;
            font-size: 8px;
            line-height: 1.2;
            color: #2c5282;
            margin-bottom: 2rem;
            white-space: pre;
            overflow-x: auto;
        }
        
        h1 {
            font-size: 2rem;
            font-weight: 700;
            color: #1a202c;
            margin-bottom: 0.5rem;
            border-bottom: 3px solid #4299e1;
            padding-bottom: 0.5rem;
        }
        
        h2 {
            font-size: 1.5rem;
            font-weight: 600;
            color: #2d3748;
            margin-top: 2rem;
            margin-bottom: 1rem;
            border-left: 4px solid #4299e1;
            padding-left: 1rem;
        }
        
        h3 {
            font-size: 1.25rem;
            font-weight: 600;
            color: #2d3748;
            margin-top: 1.5rem;
            margin-bottom: 0.75rem;
        }
        
        h4 {
            font-size: 1rem;
            font-weight: 600;
            color: #4a5568;
            margin-top: 1rem;
            margin-bottom: 0.5rem;
        }
        
        p {
            margin-bottom: 1rem;
        }
        
        .course-info {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 1rem;
            background: #edf2f7;
            padding: 1.5rem;
            border-radius: 8px;
            margin-bottom: 2rem;
        }
        
        .info-item {
            display: flex;
            flex-direction: column;
        }
        
        .info-label {
            font-weight: 600;
            color: #4a5568;
            font-size: 0.875rem;
            text-transform: uppercase;
            letter-spacing: 0.05em;
        }
        
        .info-value {
            color: #1a202c;
            font-size: 1rem;
            margin-top: 0.25rem;
        }
        
        .week-block {
            background: #f7fafc;
            border-left: 4px solid #4299e1;
            padding: 1.5rem;
            margin-bottom: 1.5rem;
            border-radius: 4px;
        }
        
        .week-header {
            font-size: 1.25rem;
            font-weight: 600;
            color: #2c5282;
            margin-bottom: 0.75rem;
        }
        
        .week-topics {
            margin-bottom: 1rem;
        }
        
        .section-label {
            font-weight: 600;
            color: #4a5568;
            margin-bottom: 0.5rem;
            text-transform: uppercase;
            font-size: 0.875rem;
            letter-spacing: 0.05em;
        }
        
        ul {
            margin-left: 1.5rem;
            margin-bottom: 1rem;
        }
        
        li {
            margin-bottom: 0.5rem;
        }
        
        .assessment-grid {
            display: grid;
            grid-template-columns: 2fr 1fr 1fr;
            gap: 1rem;
            margin-bottom: 2rem;
        }
        
        .assessment-item {
            background: #edf2f7;
            padding: 1rem;
            border-radius: 4px;
        }
        
        .assessment-item strong {
            display: block;
            color: #2c5282;
            margin-bottom: 0.5rem;
        }
        
        .callout {
            background: #bee3f8;
            border-left: 4px solid #2c5282;
            padding: 1rem;
            margin: 1rem 0;
            border-radius: 4px;
        }
        
        .materials-list {
            background: #f7fafc;
            padding: 1.5rem;
            border-radius: 4px;
            margin-bottom: 1rem;
        }
        
        .footer {
            margin-top: 3rem;
            padding-top: 1rem;
            border-top: 1px solid #e2e8f0;
            text-align: center;
            color: #718096;
            font-size: 0.875rem;
        }
        
        .page-break {
            page-break-after: always;
        }
        
        @media print {
            body {
                padding: 0.5in;
            }
            
            .page-break {
                page-break-after: always;
            }
        }
    </style>
</head>
<body>
    <div class="ascii-header">
██████╗ ██╗ ██████╗ ██╗████████╗ █████╗ ██╗     
██╔══██╗██║██╔════╝ ██║╚══██╔══╝██╔══██╗██║     
██║  ██║██║██║  ███╗██║   ██║   ███████║██║     
██║  ██║██║██║   ██║██║   ██║   ██╔══██║██║     
██████╔╝██║╚██████╔╝██║   ██║   ██║  ██║███████╗
╚═════╝ ╚═╝ ╚═════╝ ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝
████████╗ ██████╗  ██████╗ ██╗     ███████╗
╚══██╔══╝██╔═══██╗██╔═══██╗██║     ██╔════╝
   ██║   ██║   ██║██║   ██║██║     ███████╗
   ██║   ██║   ██║██║   ██║██║     ╚════██║
   ██║   ╚██████╔╝╚██████╔╝███████╗███████║
   ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝╚══════╝
    </div>

    <h1>Digital Tools for Interaction</h1>
    <p style="font-size: 1.25rem; color: #4a5568; margin-bottom: 2rem;">Module 2: Arduino & Physical Computing</p>

    <div class="course-info">
        <div class="info-item">
            <span class="info-label">Course Code</span>
            <span class="info-value">DSGN-1040</span>
        </div>
        <div class="info-item">
            <span class="info-label">Term</span>
            <span class="info-value">Winter 2026</span>
        </div>
        <div class="info-item">
            <span class="info-label">Module Dates</span>
            <span class="info-value">March 4 - April 15, 2026</span>
        </div>
        <div class="info-item">
            <span class="info-label">Schedule</span>
            <span class="info-value">Wednesdays 6:00-9:00 PM</span>
        </div>
        <div class="info-item">
            <span class="info-label">Location</span>
            <span class="info-value">A206</span>
        </div>
        <div class="info-item">
            <span class="info-label">Co-Instructor (Module 1)</span>
            <span class="info-value">Roberta Schultz</span>
        </div>
    </div>

    <h2>Instructor</h2>
    <p><strong>Nathan Ryan</strong><br>
    📧 nryan@nscad.ca<br>
    🕒 Office hours by appointment</p>

    <h2>Official Course Description</h2>
    <div class="callout" style="background: #e6fffa; border-left-color: #319795;">
        <p><strong>DSGN-1040 Digital Tools for Interaction (3 Credits)</strong></p>
        <p>Many of the experiences we have with products and objects today have some form of human-computer interaction. This interaction can involve digital software products like apps and websites (UI design) or software/hardware products like microcontroller based interactive systems (physical computing). This course introduces students to the fundamentals of interaction design and the basic processes, techniques and software tools associated with the design and development of digital products associated with apps and websites and microcontroller systems.</p>
    </div>

    <h2>Module 2 Description</h2>
    <p>Module 2 of Digital Tools for Interaction introduces students to physical computing through Arduino microcontrollers and basic electronics. Building upon the digital interface design skills developed in Module 1, students will learn to create interactive systems that respond to physical inputs with tangible outputs.</p>
    
    <p>This module emphasizes hands-on experiential learning, reverse engineering of existing devices, and the development of functional prototypes. Students will gain fundamental understanding of electronics principles, sensor integration, and output control while developing one complete interactive project.</p>

    <h2>Learning Objectives</h2>
    <p>By the end of this module, students will be able to:</p>
    <ul>
        <li><strong>Understand foundational electronics principles</strong> including voltage, current, resistance, and Ohm's law</li>
        <li><strong>Analyze existing interactive devices</strong> through systematic disassembly and component identification</li>
        <li><strong>Program Arduino microcontrollers</strong> for basic digital and analog input/output operations</li>
        <li><strong>Integrate sensors</strong> to create responsive systems that react to environmental stimuli</li>
        <li><strong>Control physical outputs</strong> including LEDs, sound generators, and servo motors</li>
        <li><strong>Design and build a functional interactive device</strong> that demonstrates integration of input, processing, and output</li>
        <li><strong>Troubleshoot circuit and code problems</strong> through systematic debugging methodologies</li>
        <li><strong>Document technical projects</strong> effectively for presentation and assessment</li>
    </ul>

    <div class="page-break"></div>

    <h2>Weekly Schedule</h2>

    <div class="week-block">
        <div class="week-header">Week 1 (March 4): Electronics Fundamentals & Technological Archaeology</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Basic electricity: voltage, current, resistance</li>
            <li>Ohm's law and circuit calculations</li>
            <li>Forward voltage drop in components</li>
            <li>Introduction to breadboarding</li>
            <li>Systematic device disassembly and analysis</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Hands-on experiments with batteries and LEDs</li>
            <li>Component identification exercises</li>
            <li>Beginning archaeological investigation of electronic devices</li>
        </ul>
        
        <div class="section-label">Assignment</div>
        <p>Complete device disassembly, document findings, prepare 2-3 minute presentation</p>
    </div>

    <div class="week-block">
        <div class="week-header">Week 2 (March 11): Archaeological Presentations & Arduino Basics</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Student presentations on device archaeology (30 minutes)</li>
            <li>Introduction to Arduino hardware and IDE</li>
            <li>Digital output programming</li>
            <li>Basic circuit construction</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Archaeological presentations</li>
            <li>Arduino IDE setup and configuration</li>
            <li>Programming first sketch (Blink)</li>
            <li>Building LED circuits on breadboard</li>
        </ul>
        
        <div class="section-label">Learning Milestone</div>
        <p>Successfully program and run Arduino sketch</p>
    </div>

    <div class="week-block">
        <div class="week-header">Week 3 (March 18): Digital Input</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Digital sensors and switches</li>
            <li>Pull-up and pull-down resistors</li>
            <li>Reading digital states</li>
            <li>Conditional programming logic</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Button and switch circuits</li>
            <li>Programming digital input responses</li>
            <li>Creating interactive LED control systems</li>
        </ul>
        
        <div class="section-label">Learning Milestone</div>
        <p>Create button-controlled output</p>
    </div>

    <div class="week-block">
        <div class="week-header">Week 4 (March 25): Analog Sensing & PWM Output</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Analog-to-digital conversion</li>
            <li>Sensor integration (photoresistors, potentiometers)</li>
            <li>Pulse Width Modulation (PWM)</li>
            <li>Sound generation with piezo elements</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Building light-sensing circuits</li>
            <li>Programming variable output responses</li>
            <li>Creating sound-reactive systems</li>
        </ul>
        
        <div class="section-label">Learning Milestone</div>
        <p>Implement analog input with variable output</p>
    </div>

    <div class="week-block">
        <div class="week-header">Week 5 (April 1): Motion Control & Project Proposals</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Servo motor control</li>
            <li>Position and movement programming</li>
            <li>Project planning and feasibility assessment</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Servo motor integration exercises</li>
            <li>Project proposal development</li>
            <li>One-on-one project consultations</li>
        </ul>
        
        <div class="section-label">Assignment</div>
        <p>Submit project proposal defining input, processing, and output components</p>
    </div>

    <div class="week-block">
        <div class="week-header">Week 6 (April 8): Open Workshop [Assessment Period]</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Individual project troubleshooting</li>
            <li>Advanced integration techniques as needed</li>
            <li>Documentation strategies</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Open workshop format</li>
            <li>Individual technical support</li>
            <li>Peer collaboration and feedback</li>
        </ul>
    </div>

    <div class="week-block">
        <div class="week-header">Week 7 (April 15): Final Project Demonstrations [Assessment Period]</div>
        
        <div class="section-label">Topics</div>
        <ul>
            <li>Project demonstration and documentation</li>
            <li>Reflection on learning outcomes</li>
        </ul>
        
        <div class="section-label">Activities</div>
        <ul>
            <li>Final project demonstrations</li>
            <li>Brief project documentation presentations</li>
            <li>Course wrap-up</li>
        </ul>
        
        <div class="section-label">Assignment</div>
        <p>Complete project demonstration and documentation</p>
    </div>

    <div class="page-break"></div>

    <h2>Assessment Structure</h2>

    <h3>1. Archaeological Investigation & Presentation (Week 2) - 30%</h3>
    <p>Students will systematically disassemble an electronic device from the 1950s-1990s, identify components, analyze interactive properties, and present findings to the class.</p>
    
    <div class="section-label">Criteria:</div>
    <ul>
        <li>Thoroughness of investigation</li>
        <li>Component identification accuracy</li>
        <li>Analysis of design decisions and interactive properties</li>
        <li>Presentation clarity and engagement</li>
    </ul>

    <h3>2. Weekly Technical Exercises (Weeks 2-5) - 20%</h3>
    <p>Progressive hands-on exercises demonstrating competency with Arduino programming basics, digital input/output, analog sensing, PWM control, and motor integration.</p>
    
    <div class="section-label">Criteria:</div>
    <ul>
        <li>Completion of weekly exercises</li>
        <li>Demonstration of understanding</li>
        <li>Engagement with troubleshooting process</li>
    </ul>

    <h3>3. Final Interactive Project (Weeks 6-7) - 50%</h3>
    <p>Students will design and build a functional interactive device using Arduino that responds to physical input with tangible output.</p>
    
    <div class="callout">
        <strong>Project Requirements:</strong>
        <ul style="margin-top: 0.5rem;">
            <li>Arduino-based system</li>
            <li>Physical input component (sensor, switch, or interface)</li>
            <li>Physical output component (visual, auditory, or kinetic)</li>
            <li>Demonstrable interactivity</li>
            <li>Brief documentation of concept and execution</li>
        </ul>
    </div>
    
    <div class="section-label">Criteria:</div>
    <ul>
        <li>Functionality and technical execution</li>
        <li>Creative application of learned concepts</li>
        <li>Problem-solving and troubleshooting demonstrated</li>
        <li>Documentation quality</li>
    </ul>

    <div class="callout" style="background: #c6f6d5; border-left-color: #38a169;">
        <strong>Grading Philosophy:</strong> Self-assessment with instructor review. Students will assess their own learning and assign a grade; instructor will not lower self-assigned grades but reserves the right to grade higher when students undervalue their work.
    </div>

    <h2>Required Materials</h2>

    <div class="materials-list">
        <h4>Electronics Kit (Material Fee: $75)</h4>
        <p>Students will purchase an electronics kit that includes all necessary components for the course. This kit becomes the student's property and can be used for future projects. Kit includes:</p>
        <ul>
            <li>Microcontroller board</li>
            <li>Breadboard and jumper wires</li>
            <li>Sensors and input devices</li>
            <li>Output components (LEDs, buzzer, servo motor)</li>
            <li>Resistors and other passive components</li>
            <li>USB cable and power supply</li>
        </ul>
    </div>

    <div class="materials-list">
        <h4>Students Provide:</h4>
        <ul>
            <li>Electronic device for archaeological investigation (Week 1)</li>
            <li>Laptop with Arduino IDE installed</li>
            <li>Safety glasses (if available; extras provided)</li>
        </ul>
    </div>

    <h2>Teaching Methodology</h2>
    <p>This course employs experiential learning methodologies prioritizing hands-on engagement with physical materials and systems. Key pedagogical approaches include:</p>
    <ul>
        <li><strong>Visceral Introduction to Concepts:</strong> Abstract principles are made concrete through direct physical experience before theoretical explanation</li>
        <li><strong>Reverse Engineering:</strong> Students learn by analyzing and understanding existing systems before creating new ones</li>
        <li><strong>Progressive Complexity:</strong> Concepts build incrementally from simple to complex, ensuring solid foundation</li>
        <li><strong>Safe Failure Environment:</strong> Students are encouraged to experiment, break components, and learn from failures</li>
        <li><strong>Practical Tolerance:</strong> Emphasis on functional understanding over engineering precision</li>
        <li><strong>Self-Directed Assessment:</strong> Students develop metacognitive skills by assessing their own learning</li>
    </ul>

    <h2>Relationship to Module 1</h2>
    <p>Module 1 (Figma) provides students with digital interface design skills and user experience thinking. Module 2 extends these concepts into physical space, allowing students to create interactive systems where digital logic controls tangible responses. The integrated course title "Digital Tools for Interaction" reflects the progression from designing interfaces to implementing functional interactive systems.</p>

    <h2>Important Policies</h2>

    <h3>Accommodation Statement</h3>
    <p>Students requiring accommodations should contact the instructor during the first week of the module to discuss needs and arrange appropriate support.</p>

    <h3>Academic Integrity</h3>
    <p>Students are expected to complete all work honestly and give proper credit for ideas and assistance received from others. Collaboration on learning exercises is encouraged; final projects should represent individual work unless otherwise specified.</p>

    <h2>Final Grade Calculation</h2>
    <div class="callout">
        <strong>DSGN-1040 Final Grade</strong> = (Module 1 Grade + Module 2 Grade) ÷ 2
        <br><br>
        <strong>Module 2 grade components:</strong>
        <ul style="margin-top: 0.5rem;">
            <li>Archaeological Investigation: 30%</li>
            <li>Weekly Exercises: 20%</li>
            <li>Final Project: 50%</li>
        </ul>
        <br>
        <strong>Grades due to Registrar:</strong> April 20, 2026
    </div>

    <div class="footer">
        <p><em>This outline is subject to minor adjustments based on student needs and learning pace. Any significant changes will be communicated to students promptly.</em></p>
        <p style="margin-top: 1rem;">NSCAD University • Digital Tools for Interaction • Winter 2026<br>
        Building interactive futures through creative technology</p>
    </div>
</body>
</html>
