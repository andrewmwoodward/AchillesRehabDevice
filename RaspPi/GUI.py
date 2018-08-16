import matplotlib.pyplot as plt
import numpy as np
import os


battFullSub = plt.imread('batteryFull.png')
battHalfSub = plt.imread('batteryHalf.png')
battEmptySub = plt.imread('batteryEmpty.png')

flexFigure = plt.imread('1.png')
versionFigure = plt.imread('2.png')

B1 = plt.imread('B1.png')
B2 = plt.imread('B2.png')
B3 = plt.imread('B3.png')
B4 = plt.imread('B4.png')

F1 = plt.imread('F1.png')
F2 = plt.imread('F2.png')
F3 = plt.imread('F3.png')
F4 = plt.imread('F4.png')

# battery subplot
ax1 = plt.subplot2grid((3,4), (0,0))
ax1.set_title('Battery Level')
ax1.axis('off')

# ankle flexion subplot
ax2 = plt.subplot2grid((3,4), (1,0), projection='polar')

# ankle in/eversion subplot
ax3 = plt.subplot2grid((3,4), (2, 0), projection='polar')

# flexion figure
ax4 = plt.subplot2grid((3,4), (1,1))
ax4.axis('off')
ax4.imshow(flexFigure)

# in/eversion figure
ax5 = plt.subplot2grid((3,4), (2, 1))
ax5.axis('off')
ax5.imshow(versionFigure)

# front foot figure subplot
ax6 = plt.subplot2grid((3,4), (1, 2))
ax6.axis('off')

# back foot figure subplot
ax7 = plt.subplot2grid((3,4), (2, 2))
ax7.axis('off')

# front foot force subplot
ax8 = plt.subplot2grid((3,4), (1, 3))

# back foot force subplot
ax9 = plt.subplot2grid((3,4), (2, 3))

# daily maximums subplot
ax10 = plt.subplot2grid((3,4), (0,1), colspan=2)

# muscle activity subplots
ax11 = plt.subplot2grid((3,4), (0,3))


muscleData = [5]*10
frontData = [5]*10
backData = [5]*10

dorsiMax = 0
plantarMax = 0
inversionMax = 0
eversionMax = 0
muscleMax = 0
frontMax = 0
backMax = 0


while 1:
#for i in range(10):
        while 1:
                f = open('value.txt', 'r')#, os.O_NONBLOCK)
                lines = f.readlines()
                if len(lines) > 0:
                        received = lines[0]
        
                        inputString = received
       
                        newData =  inputString.strip().split(',')

                        if len(newData) == 7:
                                break
        
        flexAngle = int(newData[0])
        # some realistic filters based on expected max and mins
        if flexAngle > 180:
                flexAngle = 180
        elif flexAngle < 40:
                flexAngle = 40
        
        versionAngle = int(newData[1])
        
        # some realistic filters based on expected max and mins
        #if versionAngle > 40:
        #        versionAngle = 40
        #elif versionAngle < -40:
        #        versionAngle = -40
        
        newFront = int(newData[2])
        newBack = int(newData[3])
        newMuscle = int(newData[4])
        batteryLevel = int(newData[5])
        steps = int(newData[6])
        

        if flexAngle > 90:
                plantarMax = flexAngle - 90
        
        if flexAngle < 90:
                dorsiMax = 90 - flexAngle
        
        # will probably have to adjust this once i get to school
        if versionAngle > 0:
                eversionMax = abs(versionAngle)
        
        if versionAngle < 0:
                inversionMax = versionAngle
        
        if newFront > frontMax:
                frontMax = newFront
        
        if newBack > backMax:
                backMax = newBack
        
        if newMuscle > muscleMax:
                muscleMax = newMuscle
        
        
        r = [0,1]
         
        title = 'Flexion: ' + str(flexAngle)
        if flexAngle < 60 or flexAngle > 130:
                flexColor = 'red'
        else:
                flexColor = 'green'  

        # convert to the orientation of the figure
        flexAngle = 90-flexAngle
        thetaFlex = np.deg2rad([flexAngle,flexAngle])
        ax2.clear()
        ax2.set_title(title)
        ax2.plot(thetaFlex, r, color=flexColor)
        ax2.plot(np.deg2rad([90,90]), r, color='black')
        ax2.set_rmax(1)
        ax2.set_rticks([])
        ax2.set_xticks([])
        ax2.grid(False)
        
        title = 'In/Eversion: ' + str(versionAngle)
        if versionAngle > 20 or versionAngle < -20:
                versionColor = 'red'
        else:
                versionColor = 'green'
        # convert to the orientation of the figure
        versionAngle = versionAngle - 90
        thetaVersion = np.deg2rad([versionAngle, versionAngle])
        ax3.clear()
        ax3.set_title(title)
        ax3.plot(thetaVersion, r, color=versionColor)
        ax3.plot(np.deg2rad([90,90]), r, color='black')
        ax3.set_rmax(1)
        ax3.set_rticks([])
        ax3.set_xticks([])
        ax3.grid(False)
             
        
        # front figure update
        del frontData[0]
        frontData.append(newFront)
        ax8.clear()
        ax8.plot(frontData, color='black')
        ax8.set_title('Front Foot Load')
        x1,x2,y1,y2 = ax8.axis()
        ax8.axis((x1,x2,0,150))
        
        if newFront < 20:
                ax6.imshow(F1)
        elif newFront < 50:
                ax6.imshow(F2)
        elif newFront < 60:
                ax6.imshow(F3)
        else:
                ax6.imshow(F4)
        
        
        # back figure update
        del backData[0]
        backData.append(newBack)
        ax9.clear()
        ax9.plot(backData, color='black')
        ax9.set_title('Back Foot Load')
        x1,x2,y1,y2 = ax9.axis()
        ax9.axis((x1,x2,0,150))
        
        if newBack < 20:
                ax7.imshow(B1)
        elif newBack < 50:
                ax7.imshow(B2)
        elif newBack < 60:
                ax7.imshow(B3)
        else:
                ax7.imshow(B4)
        
        # 11
        del muscleData[0]
        muscleData.append(newMuscle)
        ax11.clear()
        ax11.plot(muscleData, color='black')
        ax11.set_title('Muscle Activity')
        x1,x2,y1,y2 = ax11.axis()
        ax11.axis((x1,x2,0,100))
        
     
        if batteryLevel > 75:
                ax1.imshow(battFullSub)
        elif batteryLevel > 25:
                ax1.imshow(battHalfSub)
        else:
                ax1.imshow(battEmptySub)
                
        ax10.clear()
        ax10.axis('off')
        ax10.set_title("Daily Maximums")
        
        ax10.text(0,0.8, 'Dorsi: ' + str(dorsiMax), fontsize=9)
        ax10.text(0,0.6, 'Plantar: ' +str(plantarMax), fontsize=9)
        ax10.text(0,0.4, 'Inversion: ' + str(abs(inversionMax)), fontsize=9)
        ax10.text(0,0.2, 'Eversion: ' + str(abs(eversionMax)), fontsize=9)
        ax10.text(0.5,0.8, 'Muscle Activity: ' + str(muscleMax), fontsize=9)
        ax10.text(0.5,0.6, 'Front Foot Load: ' + str(frontMax), fontsize=9)
        ax10.text(0.5,0.4, 'Back Foot Load: ' + str(backMax), fontsize=9)
        ax10.text(0.5,0.2, 'Steps: ' + str(steps), fontsize=9)
                
        plt.tight_layout()
        plt.draw()
        plt.pause(1e-17)

plt.show()

