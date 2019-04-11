from mininet.node import CPULimitedHost
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel, info
from mininet.node import Controller, RemoteController
from mininet.cli import CLI

from mininet.term import makeTerm, cleanUpScreens
from time import sleep

extendedArchitecture = True

websiteIP = "100.000.000.001"
websitePortForParent = 20008
websitePortForChild = 20009
parentIP = "100.000.000.002"
childIP = "100.000.000.003"

websiteIPUsedByParent = parentIP if extendedArchitecture else websiteIP
websiteIPUsedByChild = childIP if extendedArchitecture else websiteIP






switchName = "thirdParty"

class Topology(Topo):
    
    def build(self):
        global ifaceID
    

    
        # Add hosts
        parent = self.addHost("parent", ip=parentIP)
        child = self.addHost("child", ip=childIP)
        website = self.addHost("website", ip=websiteIP)
        
        # Add common switch on which the third party will run
        thirdParty = self.addSwitch(switchName, dpid="0000000000000001")

        # Add links, create star topology
        self.addLink(thirdParty, website)
        self.addLink(thirdParty, parent)
        self.addLink(thirdParty, child)


def run():
    topo = Topology()
    net = Mininet(topo)
    net.start()
    
    ifaceID = net.getNodeByName(switchName).intfNames()[1]

    makeTerm(node=net.getNodeByName("website"), title="Website", cmd="./Website {} {}".format(websitePortForParent, websitePortForChild))
    
    if extendedArchitecture:
        makeTerm(node=net.getNodeByName("website"), title="IWebsite", cmd="./IWebsite")
        makeTerm(node=net.getNodeByName("website"), title="OWebsite", cmd="./OWebsite")
        makeTerm(node=net.getNodeByName("website"), title="RWebsite", cmd="./RWebsite {} {}".format(websitePortForParent, websitePortForChild))
        sleep(1)
        makeTerm(node=net.getNodeByName("child"), title="OChild", cmd="./OChild {}".format(websiteIP))
        sleep(1)
        makeTerm(node=net.getNodeByName("child"), title="RChild", cmd="./RChild {}".format(websitePortForChild))
        sleep(1)

    #sleep(2)
    #makeTerm(node=net.getNodeByName("parent"), title="Parent", cmd="./Parent {} {}".format(websiteIPUsedByParent, websitePortForParent))
    makeTerm(node=net.getNodeByName("child"), title="Child", cmd="./Child {} {}".format(websiteIPUsedByChild, websitePortForChild))
    #makeTerm(node=net.getNodeByName(switchName), title="Third Party", cmd="./ThirdParty {} {} {} {}".format(ifaceID, websiteIP, parentIP, childIP))


    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    run()
