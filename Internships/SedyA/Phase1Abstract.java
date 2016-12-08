package sedya.apps;

import ducks.driver.SeDyANode;
import ducks.driver.SeDyANodes;
import ducks.driver.SeDyASimParams;
import ducks.misc.GlobalConfig;
import ext.jist.swans.misc.NodeDatabase.NodeInfo;
import ext.util.PropertyNotFoundException;
import ext.util.stats.StatsManager;
import ext.util.stats.StatsManager.AdditiveLongValue;
import ext.util.stats.StatsManager.ResultValue;
import ext.util.stats.StatsProvider;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import jist.runtime.JistAPI;
import jist.swans.field.Field.RadioData;
import jist.swans.mac.MacAddress;
import jist.swans.misc.Location;
import jist.swans.misc.Location.Location2D;
import jist.swans.misc.Message;
import jist.swans.misc.Protocol;
import jist.swans.net.NetAddress;
import jist.swans.net.NetIpBase;
import sedya.data.Identity;
import sedya.data.Phase1Message;

public abstract class Phase1Abstract extends SeDyAApp implements StatsProvider, Phase1ProxyInterface{

    private static final long MS = jist.swans.Constants.MILLI_SECOND;
    private Location2D topRight;

    public Location getTopRight() {
        return topRight;
    }

    /**
     * Initialize the App that represents phase 1. None of the values passed
     * will ever be modified in the newly created object.
     * @param nodeId unique id of this node.
     * @param nt neighbor table reference.
     * @param rd RadioData reference.
     */
    public Phase1Abstract(SeDyANode n, int nodeId, NeighborTable nt, RadioData rd, Location2D[] l) {
        super(n, nodeId, nt, rd);
        this.topRight = l[3];
    }
    /** Current message. Represents the current aggregate.*/
    protected Phase1Message curr;
    /** time stamp of the START of the previous observation.*/
    protected long lastRun = -2 * SeDyAApp.PHASE1_PERIOD;
    protected Location prevLoc = null;
    protected long prevTime;
    /** This Location indicates the location where the most recent observation
    was made.*/
    public Location observationLocation = null;
    /** This double indicates the speed of the most recent observation. */
    public int observationSpeed;

    public static final int FWD_PROBABILITY = 50;
    public static final int MESSAGE_LIMIT = Phase2Abstract.MESSAGE_LIMIT; //x/NTsize messages sent per node per second
    
    
    protected int data1 = 0;

    private boolean pause=false;
    
    /**
     * The actual phase. Will be bytecode-rewritten.
     */
    @Override
    protected void run() {
        //first run?
        if (JistAPI.getTime() == 0) {
            timer();
            //wait for a few beacon periods and a bit, then start
            long delay = MS * 400 + SeDyAApp.PHASE1_RANDOM.nextInt(SeDyAApp.PHASE1_JITTER);
            this.prevLoc = this.rd.getLoc();
            this.prevTime = JistAPI.getTime();
            //schedule for current time + delay
            JistAPI.sleep(delay);
            ((Protocol) self).start();
            return;
        } else 
        //already ran this observation period?
        if (JistAPI.getTime() - lastRun < SeDyAApp.PHASE1_PERIOD) {
            //wait for the next one
            long delay = SeDyAApp.PHASE1_PERIOD / 8 + SeDyAApp.PHASE1_RANDOM.nextInt(SeDyAApp.PHASE1_JITTER);
            //schedule for current time + delay
            JistAPI.sleep(delay);
            ((Protocol) self).start();
            return;
        } else //we are now in an observation period
        {
            pause=false;
            long delay = SeDyAApp.PHASE1_RANDOM.nextInt(SeDyAApp.PHASE1_JITTER);

            //update the timestamp of last run first
            this.lastRun = JistAPI.getTime();

            //perform observation
            Phase1Message m = observe();
            this.node.upbaseObsLog(JistAPI.getTime(), Phase1Message.generateObservation(this.observationSpeed, this.observationLocation));


            this.prevLoc = this.rd.getLoc();
            this.prevTime = JistAPI.getTime();

            //for now, just sign everything in one go and use a new pseudonym every
            //aggregate.
            Identity ident = this.node.getId();

            while (m.sign(ident));
            //spend time for signing? then counter + JistAPI.runAt call here

            //if there is a current message already for this time block, and we can
            //merge it with our observation, then do so.
            //TODO: merge before so we can save signature operations (?)-> too complicated, and no time is spent signing anyway.
            Phase1Message tmp = null;
            if (curr != null && curr.getTimeBlock() == m.getTimeBlock() && Phase1Message.compatible(m, curr)) {

                if (SeDyAApp.securityEnabled && 
                        (!compareWithNeighborTable(m) ||
                        StrictMath.abs(this.node.getSpeedReading() - curr.estimate()) > AGREEMENT_THRESHOLD)) {
                    //do not merge; replace the current message with our own observation.
                    tmp = m;
                    if (SeDyANodes.verbose) {
                        System.err.println("Phase1Obs@" + this.id + ": NTError");
                    }
                } else {
                    if (SeDyANodes.verbose) {
                        System.err.println("merge@" + id + ": " + curr.estimate() + " with our observation " + observationSpeed + " represented in a sketch with " + m.estimate());
                    }
                    tmp = curr.merge(m);
                }
            }
            if (tmp != null) {
                m = tmp;
            }

            //send stuff
            //System.err.println("Generated message @" + this.id + "(mid=" + m.id + "|"+(curr!=null?curr.id:"")+"): " + "(t=" + JistAPI.getTime() + "-->" + m.getTimeBlock() + ")" + m.getObservations() + " | " + m.getParticipants());
            curr = m;

            if (m.getSize() < NetIpBase.MAXIMUM_PAYLOAD_SIZE && this.counter < MESSAGE_LIMIT) {
                this.counter++;
                doStats(m);
                netEntity.send(m, NetAddress.ANY, (short) (vans.Constants.NET_PROTOCOL_SEDYA + SeDyAApp.PHASE1), jist.swans.Constants.NET_PRIORITY_NORMAL, (byte) 1);
            } else {
                //System.err.println("Warning, message too large or too frequent.");
            }

            //call self.
            JistAPI.sleep(delay);
            ((Protocol) self).start();
        }
    }
    
    @Override
    protected void proxySelf(){
        this.self = JistAPI.proxy(this, Phase1ProxyInterface.class);
    }
    
    private int counter=0;
    @Override
    public void timer(){
        counter=counter-Math.max((int)Math.ceil(MESSAGE_LIMIT/(Math.max(this.neighborTable.getSize(), 1))),1);
        if (counter<0)
            counter=0;
        
        JistAPI.sleep(1000*MS + jist.swans.Constants.MICRO_SECOND * SeDyAApp.PHASE1_RANDOM.nextInt(SeDyAApp.PHASE1_JITTER));
        ((Phase1ProxyInterface)self).timer();
    }

    /**
     * Perform an observation. Determines:
     * - fixed point
     * - data (speed observation)
     * - distance from fixed point
     * @return A message that represents the observation.
     */
    protected Phase1Message observe() {
        Location l = rd.getLoc();
        //TODO multiple readings + average..?

        //select fixed point using integer division:
        int fX = (int) (l.getX() / ((int) SeDyAApp.FIXED_POSITION_INTERVAL_X));
        int fY = (int) (l.getY() / ((int) SeDyAApp.FIXED_POSITION_INTERVAL_Y));
        fX *= SeDyAApp.FIXED_POSITION_INTERVAL_X;
        fY *= SeDyAApp.FIXED_POSITION_INTERVAL_Y;

        //payload will be speed, the issue is how to obtain it from a sensor
        int data = this.node.getSpeedReading();

        int x = -1, y = 0; //this assumes static area and no highway.

        if (GlobalConfig.instance().getConfig().getBooleanProperty(SeDyASimParams.SEDYA_STATIC_AREA, true)) {
            //static area, so the center is + 0.5 *(size)
            //for static areas, the fixed point should be the center to simplify
            //a lot of computations
            fX += (int) (0.5 * SeDyAApp.FIXED_POSITION_INTERVAL_X);
            fY += (int) (0.5 * SeDyAApp.FIXED_POSITION_INTERVAL_Y);
        } else {
            //dynamic area, so set the distance from the fixed point
            x = (int) (l.getX() - fX);
            y = (int) (l.getY() - fY);
        }
        //generate timestamp
        int time = SeDyAApp.getCurrentTimeBlock();

        data1 = data;
        this.observationLocation = l;
        this.observationSpeed = data;
        
        //create Phase1Message:
        try {
            if (GlobalConfig.instance().getConfig().getBooleanProperty(SeDyASimParams.SEDYA_HIGHWAY, false)) {
                //highway scenario: always on x axis
                return new Phase1Message(data, new Location2D(fX, fY), new Location2D(x, -1), this.id, time);
            } else {
                return new Phase1Message(data, new Location2D(fX, fY), new Location2D(x, y), this.id, time);
            }
        } catch (PropertyNotFoundException e) {
            throw new RuntimeException("Error parsing parameters!\n" + e.getMessage());
        }
    }
    
    /**
     * Receive a message from the Network layer. This will be called when Phase1
     * is registered as handler for Phase1Messages.
     * @see SeDyANode#addSeDyA()
     * @param msg the message. Will be a Phase1Message instance.
     * @param src source IP.
     * @param lastHop MAC of last hop (=source).
     * @param macId identifier of the MAC interface.
     * @param dst destination IP. Should be NetAddress.ANY.
     * @param priority priority of the message.
     * @param ttl ttl of the message (=1).
     */
    @Override
    public void receive(Message msg, NetAddress src, MacAddress lastHop,
            byte macId, NetAddress dst, byte priority, byte ttl) {
        if(pause)
            return;
        if (msg instanceof Phase1Message) {
            Phase1Message m = (Phase1Message) msg;

            //legitimacy checks
            if ( SeDyAApp.securityEnabled && !m.phyModelChecks()) {
                System.err.println("Phase1Rcv@" + this.id + ": PHYError");
                return; //drop: physical model checks failed.
            }
            if ( SeDyAApp.securityEnabled && curr != null && m.getTimeBlock() < curr.getTimeBlock()) {
                System.err.println("Phase1Rcv@" + this.id + ": TimeError");
                return; //drop: older than our message.
            }
            if (SeDyAApp.securityEnabled && (!compareWithNeighborTable(m) || StrictMath.abs(this.observationSpeed - m.estimate()) > AGREEMENT_THRESHOLD)) {
                if (SeDyANodes.verbose) {
                    double ntavg = 0.0;
                    for (NodeInfo ni : this.neighborTable) {
                        NodeDataSpeed nds = (NodeDataSpeed) ni.getData(NodeDataSpeed.NODE_DATA_ID);

                        if (false) {
                            SpeedContainer sc = nds.getLatestSpeed();
                            if (sc != null) { // if sc is null, there is no recent speed entry
                                ntavg += sc.getSpeed();
                            }
                        } else {
                            ntavg += nds.getAverageSpeed();
                        }
                    }
                    if (SeDyANodes.verbose) {
                        System.err.println(m.estimate() + " (" + src + ") but actually its more like " + this.observationSpeed + ", but NT says " + (ntavg / this.neighborTable.getSize()));
                    }
                    if (!compareWithNeighborTable(m)) {
                        if (SeDyANodes.verbose) {
                            System.err.println("Phase1Rcv@" + this.id + ": NTError");
                        }
                    }
                    if (StrictMath.abs(this.observationSpeed - m.estimate()) > AGREEMENT_THRESHOLD) {
                        if (SeDyANodes.verbose) {
                            System.err.println("Phase1Rcv@" + this.id + ": MyError");
                        }
                    }

                }
                return; //drop: this node disagrees with aggregate or the aggregate represents another area.
            }else if (SeDyANodes.verbose) {
                        System.err.println(m.estimate() + " (" + src + ") but actually its more like " + this.observationSpeed );
                    }
            if (SeDyAApp.securityEnabled && !m.sigVerify()) {
                System.err.println("Phase1Rcv@" + this.id + ": SigError (src=" + src + ")");
                return; //drop: signature verification failure
            }
            
            //node considers message legit now.
            //if we don't have an aggregate yet, or ours is old, then store it
            if (curr == null || m.getTimeBlock() > curr.getTimeBlock()) {
                curr = m.merge(m); // create a copy of m
            }

            if (Phase1Message.compatible(curr, m)) {
                //merge & rebroadcast
                if (curr != m && !emptyNeighborTable()) {

                    curr = curr.merge(m);
                }

                if(GlobalConfig.instance().getConfig().getBooleanProperty(SeDyASimParams.SEDYA_STATIC_AREA, false)){
                    //static area case: if no longer in area, then start forwarding.
                    Location[] x = new Location[1];
                    x[0]=this.curr.getEstimateLocation();
                    if (!SeDyANodes.containedInStaticArea(x, this.rd.getLoc()) ){
                        notifyDisagreementListeners(curr, src, lastHop);
                        return;
                    }
                }

                if (isEdgeNode(prevLoc, this.rd.getLocByRef())) {
                    notifyDisagreementListeners(curr, src, lastHop);
                    pause=true;
                    return; //drop: this node disagrees with aggregate or the aggregate represents another area.
                }

                //System.err.println("Generated message @" + this.id + "(mid="+(curr!=null?curr.id:"")+"): " + "(t=" + JistAPI.getTime() + "-->" + curr.getTimeBlock() + ")" + curr.getObservations() + " | " + curr.getParticipants());
                if (curr.getSize() < NetIpBase.MAXIMUM_PAYLOAD_SIZE) {
                    if(this.counter >= MESSAGE_LIMIT){
                        queue(curr);
                    }else{
                        doStats(curr);
                        this.counter++;
                        netEntity.send(curr, NetAddress.ANY, (short) (vans.Constants.NET_PROTOCOL_SEDYA + SeDyAApp.PHASE1), jist.swans.Constants.NET_PRIORITY_NORMAL, (byte) 1);
                    }
                } else {
                    //message too large
                }
            } else {
                //messages incompatible
                if (SeDyANodes.verbose) {
                    System.err.println("Phase1Rcv@" + this.id + ", t=" + getCurrentTimeBlock() + ": incompatible: " + curr.getTimeBlock() + " " + m.getTimeBlock() + " " + curr.getLocation() + " " + m.getLocation());
                }
            }
        } else { //not a Phase1Message instance
            throw new UnsupportedOperationException("Registered Phase1 instance for non-Phase1Message instance: " + msg.getClass());
        }
    }
  