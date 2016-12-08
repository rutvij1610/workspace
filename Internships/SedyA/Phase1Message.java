package sedya.data;

import ducks.driver.SeDyANodes;
import ducks.misc.GlobalConfig;
import sketches.AMFM_PCSASketch;
import sketches.AMLCSketch;
import sketches.AMSketch;
import sedya.apps.SeDyAApp;
import ducks.driver.SeDyASimParams;
import ext.util.ExtendedProperties;
import ext.util.PropertyNotFoundException;
import jist.swans.misc.Location;
import jist.swans.misc.Location.Location2D;
import jist.swans.net.NetIpBase;


public class Phase1Message implements SeDyAApp.SeDyAMessage {

    public static final int AREA_FACTOR = 10; // 10000/1000 --> at most 2000 hash operations for location

    public static Observation generateObservation(int speed, Location loc){
        return new Observation(speed, loc.getX(), loc.getY());
    }

    public void integrityCheck() {
        ((AMLCSketch)this.count).integrityCheck();
        ((AMFM_PCSASketch)this.locX).verifyAll();
    }
    public static class Observation {

        public Observation(int v, double x, double y) {
            this.v = v;
            this.x = x;
            this.y = y;
        }
        public int v;
        public double x, y;

        @Override
        public String toString() {
            return v + "@(" + x + ", " + y + ")";
        }

        @Override
        public int hashCode(){
            return v+(int)((x+y)*10);
        }
        @Override
        public boolean equals(Object o){
            if(o instanceof Observation){
            Observation other=(Observation) o;
            return this.v==other.v&&this.x==other.x&&this.y==other.v;
            }else return false;
        }
    }

    /* integers for giving each message a unique ID. Should not be used by nodes! */
    private static int intc = 0;
    public final int id = (intc++);
    public static final ExtendedProperties config = GlobalConfig.instance().getConfig();
    /**Payload; the actual data. Eg. sum of speeds.*/
    protected final AMSketch payload;
    /**location from fixedPoint; x axis.*/
    protected final AMSketch locX;
    /**location from fixedPoint; y axis.*/
    protected final AMSketch locY;
    /**Amount of vehicles participating in this phase iteration.*/
    protected final AMSketch count;
    /**Position from which (locX, locY) determine the center of the aggregation area.*/
    protected final Location fixedPoint;
    /**Time block. Could be made into sketches as well, but cumbersome and not enough gain.*/
    protected final int timeBlock;
    /**Whether the security payload is stripped.*/
    protected boolean stripped = false;

    /**
     * Create a new message that sums.
     * @param payload current value.
     * @param fix the fixed point. this is the center for the static area setting.
     * @param xDiff fix+x; center of aggregation area. -1 for static area.
     * @param yDiff fix+y; center of aggregation area. -1 for highway.
     * @param id unique identity/pseudonymity of this vehicle.
     * @param time time block.
     * @throws Exception when a query for a configuration setting fails.
     */
    public Phase1Message(int payload, Location fix, Location diff,
            int id, int time) throws PropertyNotFoundException {
        int xDiff = (int) diff.getX();
        int yDiff = (int) diff.getY();
        //fetch parameters
        int cs = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_COUNT_SIZE);
        String ch = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_COUNT_HASH);

        int ll = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_LOC_LENGTH);
        int la = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_LOC_AMOUNT);
        String lpcsa = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_LOC_OUTERHASH);
        String lfm = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_LOC_INNERHASH);

        int pl = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_I_PAYLOAD_LENGTH);
        int pa = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_I_PAYLOAD_AMOUNT);
        String ppcsa = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_I_PAYLOAD_OUTERHASH);
        String pfm = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_I_PAYLOAD_INNERHASH);

        //construct message
        this.fixedPoint = fix;
        this.count = new AMLCSketch(cs, ch, true);
        if (xDiff >= 0) {
            this.locX = new AMFM_PCSASketch(la, ll, lfm, lpcsa, false);
            this.locX.addValue(id, xDiff / AREA_FACTOR);
        } else {
            this.locX = null;
        }
        this.payload = new AMFM_PCSASketch(pa, pl, pfm, ppcsa, false);
        this.timeBlock = time;

        this.count.count(id);
        this.payload.addValue(id, payload);
        if (yDiff >= 0) {
            //urban case
            this.locY = new AMFM_PCSASketch(la, ll, lfm, lpcsa, false);
            this.locY.addValue(id, yDiff / AREA_FACTOR);
        } else {
            this.locY = null;
        }
    }

    /**
     * Create a new message that counts.
     * @param payload current value. If true, node will add itself.
     * @param fix the fixed point.
     * @param xDiff fix+x; center of aggregation area.
     * @param yDiff fix+y; center of aggregation area. -1 for highway.
     * @param id unique identity/pseudonymity of this vehicle.
     * @param time time block.
     * @throws Exception when a query for a configuration setting fails
     */
    public Phase1Message(boolean payload, Location fix, Location diff,
            int id, int time) throws PropertyNotFoundException {
        int xDiff = (int) diff.getX();
        int yDiff = (int) diff.getY();
        //fetch parameters
        int cs = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_COUNT_SIZE);
        String ch = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_COUNT_HASH);

        int ll = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_LOC_LENGTH);
        int la = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_LOC_AMOUNT);
        String lpcsa = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_LOC_OUTERHASH);
        String lfm = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_LOC_INNERHASH);

        int ps = config.getIntProperty(SeDyASimParams.SEDYA_PHASE1_B_PAYLOAD_SIZE);
        String ph = config.getStringProperty(SeDyASimParams.SEDYA_PHASE1_B_PAYLOAD_HASH);

        //construct message
        this.fixedPoint = fix;
        this.count = new AMLCSketch(cs, ch, true);
        if (xDiff >= 0) {
            this.locX = new AMFM_PCSASketch(la, ll, lfm, lpcsa, false);
            this.locX.addValue(id, xDiff / AREA_FACTOR);
        } else {
            this.locX = null;
        }
        this.payload = new AMLCSketch(ps, ph, true);
        this.timeBlock = time;

        this.count.count(id);
        if (payload) {
            this.payload.count(id);
        }
        if (yDiff >= 0) {
            //urban case
            this.locY = new AMFM_PCSASketch(la, ll, lfm, lpcsa, false);
            this.locY.addValue(id, yDiff / AREA_FACTOR);
        } else {
            this.locY = null;
        }

    }

    /**
     * Create a new message from an existing message (eg. merge). This will not
     * copy any of the arguments.
     * @param p payload
     * @param x x sketch
     * @param y y sketch (null for highway)
     * @param c vehicle count sketch
     * @param f fixed point
     * @param time time block
     * @param stripped whether the security payload is stripped.
     */
    protected Phase1Message(AMSketch p, AMSketch x, AMSketch y, AMSketch c,
            Location f, int time, boolean stripped) {
        this.payload = p;
        this.locX = x;
        this.locY = y;
        this.count = c;
        this.fixedPoint = f;
        this.timeBlock = time;
        this.stripped = stripped;
    }

    /**
     * Create a new message from an existing message (eg. merge). This will not
     * copy any of the arguments.
     * @param p payload
     * @param x x sketch
     * @param y y sketch (null for highway)
     * @param c vehicle count sketch
     * @param f fixed point
     * @param time time block
     */
    protected Phase1Message(AMSketch p, AMSketch x, AMSketch y, AMSketch c,
            Location f, int time) {
        this.payload = p;
        this.locX = x;
        this.locY = y;
        this.count = c;
        this.fixedPoint = f;
        this.timeBlock = time;
    }

    /**
     * Update this message. Returns a merged copy iff changes have been made.
     * @see constructors of this class.
     * @return
     * @throws Exception when a query for a configuration setting fails
     */
    public Phase1Message update(int payload, Location fix, Location diff,
            int id, int time) throws PropertyNotFoundException {
        Phase1Message merged = this.merge(new Phase1Message(payload, fix, diff, id, time));
        return (merged.equals(this)) ? this : merged;
    }

    /**
     * Update this message. Returns a merged copy iff changes have been made.
     * @see constructors of this class.
     * @throws Exception when a query for a configuration setting fails
     */
    public Phase1Message update(boolean payload, Location fix, Location diff,
            int id, int time) throws PropertyNotFoundException {
        Phase1Message merged = this.merge(new Phase1Message(payload, fix, diff, id, time));
        return (merged.equals(this)) ? this : merged;
    }

    /**
     * Merge this message with the other. Will use the node instance known to
     * <b>this</b> to query simulation parameters from.
     * Will not modify contents of either existing message.
     *
     * @param other another message.
     * @return null if the messages are incompatible.
     */
    public Phase1Message merge(Phase1Message other) {
        if (!compatible(this, other)) {
            System.err.println("incompatible messages!");
            return null;
        }
        AMSketch p = payload.merge(other.payload);
        AMSketch X = null;
        if (locX != null) {
            X = locX.merge(other.locX);
        }
        AMSketch Y = null;
        if (locY != null) {
            Y = locY.merge(other.locY);
        }
        //System.err.println("merging: " + this.count + "\nwith:" + other.count);
        AMSketch c = count.merge(other.count);
        //System.err.println("result of merge is " + c);
        Phase1Message result = new Phase1Message(p, X, Y, c, fixedPoint, timeBlock);
        if (this instanceof FakePhase1Message || other instanceof FakePhase1Message) {
            result.attacked();
        }
        return result;
    }

    /**
     * Checks if two messages can be merged.
     * 'can' refers to semantic correctness here.
     * If (a, b) are compatible then merging them will not hurt verification.
     */
    public static boolean compatible(Phase1Message a, Phase1Message b) {
        /*if (!b.fixedPoint.equals(a.fixedPoint)) {
            System.err.println("fixed points don't match: " + a.fixedPoint + " " + b.fixedPoint);
        }
        if (b.timeBlock != a.timeBlock) {
            System.err.println("time blocks don't match: " + a.timeBlock + " " + b.timeBlock);
        }*/
        return b.fixedPoint.equals(a.fixedPoint) && b.timeBlock == a.timeBlock;
    }

    /**
     * Signs a single unsigned bit with the given identity.
     * @param id the identity with which the signature is generated.
     * @return true if a bit was signed.
     */
    public boolean sign(Identity id) {
        if(!SeDyANodes.usePhase1Security)
           return false; //no signatures needed
        if (!this.payload.addSignature(id)) //sign payload
        {
            if (!this.count.addSignature(id))//sign count
            {
                if (this.locX == null || !this.locX.addSignature(id)) //sign location (X)
                {
                    if (this.locY == null || !this.locY.addSignature(id)) //sign location (Y) if any
                    {
                        this.sigVerify();
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * Strips the security payload.
     * This is normally done at the end of phase 2, when the security payload is
     * no longer of any value.
     * The only thing that actually happens is that the payload is no longer
     * considered by getSize()
     */
    public Phase1Message stripSecurityPayload() {
        return new Phase1Message(payload, locX, locY, count, fixedPoint, timeBlock, true);
    }

    /**
     * Returns the size of this message, including security payload (if it is
     * not stripped).
     */
    @Override
    public int getSize() {
        int res = 0; // in bits
        res += 8; //for definition of payload.
        res += this.count.getLength();
        if (this.locX != null) {
            res += this.locX.getLength();
        }
        res += this.payload.getLength();
        if (this.locY != null) {
            res += this.locY.getLength();
        }
        res += Float.SIZE * 2 / 8; //fixed point size.
        //System.err.print("size for payload");
        if (stripped || !SeDyAApp.securityEnabled || !SeDyANodes.usePhase1Security) {//ignore security overhead, its been stripped
            res = res / 8 + ((res % 8 == 0) ? 0 : 1); //round up
            //System.err.println(" without security: "+res);
            return res;
        } else {
            res = res / 8 + ((res % 8 == 0) ? 0 : 1); //round up
            //res is now in bytes.
            res += this.count.getSignatureCount() * Signature.SIZE;
            res += this.count.getCertificateCount() * Identity.SIZE;

            if(locX!=null){
            res += this.locX.getSignatureCount() * Signature.SIZE;
            res += this.locX.getCertificateCount() * Identity.SIZE;
            }
            if(locY!=null){
            res += this.locY.getSignatureCount() * Signature.SIZE;
            res += this.locY.getCertificateCount() * Identity.SIZE;
            }
            if(payload!=null){
            res += this.payload.getSignatureCount() * Signature.SIZE;
            res += this.payload.getCertificateCount() * Identity.SIZE;
            }
            //System.err.println(" with security: "+res);
            return res;
        }
    }

    /**
     * Not implemented. Will return without modification of message.
     */
    public void getBytes(byte[] msg, int offset) {
        return;
    }

    /**
     * Perform checks against a physical model.
     * @return true if checks are passed; false otherwise.
     */
    public boolean phyModelChecks() {
        //we have no good physical models; if we did, code would be here.
        //physical models are intended to pick out values that are completely
        //impossible, so this is not a strong limitation.
        return true; 
    }

    /**
     * Verify the signatures in this message.
     * @return true if all signatures verify.
     * @throws UnsupportedOperationException when the security payload is
     * stripped.
     */
    public boolean sigVerify() {
        if (stripped) {
            throw new UnsupportedOperationException("Cannot verify signatures when security payload has been stripped.");
        } else if(!SeDyANodes.usePhase1Security){
            return true;
        } else  {
            //int sigs = this.payload.getSignatureCount() + this.count.getSignatureCount() + ((this.locX == null) ? 0 :this.locX.getSignatureCount()) + ((this.locY == null) ? 0 : this.locY.getSignatureCount());
            
            boolean result = true;
            try{
            result &=this.payload.verifyAll();
            }catch (NullPointerException e){
                result=false;
            }
            if(!result){
                System.err.println("payload");
                printit();
                System.err.println("-----------");
                for (int i= 0; i < 1000000; i++) {

                }
                throw new RuntimeException();
            }
            try{
            result &=this.count.verifyAll();
            }catch (NullPointerException e){
                result=false;
            }
            if(!result){
                System.err.println("count");
                System.err.println(count.toString());
                System.err.println("-----------");
                throw new RuntimeException();
            }
            try{
            result &=(this.locX == null || this.locX.verifyAll());
            }catch (NullPointerException e){
                result=false;
            }
            if(!result){
                System.err.println("locX");
                printit();
                System.err.println("-----------");
                throw new RuntimeException();
            }
            try{
            result &=(this.locY == null || this.locY.verifyAll());
            }catch (NullPointerException e){
                result=false;
            }
            return result;
        }
    }

    /**
     * check whether any phase1 security material exists in this message
     */
    public boolean isStripped() {
        return stripped || !SeDyANodes.usePhase1Security;
    }

    /**
     * Check if this message agrees with the given average.
     * @param avg the average
     * @see SeDyAApp#AGREEMENT_THRESHOLD
     */
    public boolean agreesWithNode(float avg) {
        double e = this.estimate();
        boolean res = Math.abs(avg - e) < SeDyAApp.AGREEMENT_THRESHOLD;
        //*
        //if (!res) {
        //System.err.print("avg: " + avg + " payload: " + e);
        //System.err.println(" sketch: " + this.payload.toString());
        //}//*/
        return res;
    }

    /**
     * Estimate the value of the payload. This is either an average
     * (for discrete inputs) or a count (for binary inputs).
     * @return
     */
    public double estimate() {
        //it would be nicer to use a boolean for this..
        if (this.payload instanceof AMFM_PCSASketch) {
            if (this.count.estimate() < 1.0) {
                return this.payload.estimate();
            } else {
                /*int sum = 0;
                for (Integer i : this.observationMap.keySet()) {
                    sum += this.observationMap.get(i).v;
                }
                //System.err.format("%f/%f=%f, actual: %d/%d=%f\n", this.payload.estimate(), this.count.estimate(), this.payload.estimate()/this.count.estimate(), sum, this.observationMap.keySet().size(), ((double)sum)/ this.observationMap.keySet().size());
                */
                return ((double) this.payload.estimate()) / ((double) this.count.estimate());
            }
        } else if (this.payload instanceof AMLCSketch) {
            return ((double) this.payload.estimate());
        } else {
            return -1.0;
        }
    }

    /**
     * Fetch the time block of this message.
     */
    public int getTimeBlock() {
        return this.timeBlock;
    }

    /**
     * Fetch the fixed point of this message.
     */
    public Location getLocation() {
        return this.fixedPoint;
    }

    /**
     * Is this message describing a highway?
     * @return
     */
    public boolean isHighway() {
        return locY == null;
    }

    /**
     * Is the message using a fixed size system?
     * @return
     */
    public boolean isFixed(){
        return locX==null && locY==null;
    }

    /**
     * toString of this message. Mostly implemented for debugging purposes.
     * @return a string with information about this message.
     * Currently: "msgid=%d@%s" with this.id and this.fixedPoint
     */
    @Override
    public String toString() {
        return "msgid=" + this.id + "@" + this.fixedPoint;
    }

    /**
     * Get the (absolute, i.e. not with respect to the fixed point)
     * center of this aggregate.
     */
    public Location getEstimateLocation() {
        double c = this.count.estimate();
        double x = (this.locX == null) ? 0 : this.locX.estimate() * AREA_FACTOR;
        double y = (this.locY == null) ? 0 : this.locY.estimate() * AREA_FACTOR;
        if (c == 0) {
            return this.fixedPoint.add(new Location2D((float) x, (float) y));
        } else {
            return this.fixedPoint.add(new Location2D((float) (x / c), (float) (y / c)));
        }
    }

    public Phase1Message attack(Identity id, boolean incr) {
        Phase1Message result=null;
        if(SeDyANodes.usePhase1Security){
            AMSketch p = null;
            if(incr)
                p = payload.attackIncr(id);
            else
                p = payload.attackDecr(id);
            AMSketch X = null;
            if (locX != null) {
                if(incr)
                    X = locX.attackIncr(id);
                else
                    X = locX.attackDecr(id);
            }
            AMSketch Y = null;
            if (locY != null) {
                if(incr)
                    Y = locY.attackIncr(id);
                else
                    Y = locY.attackDecr(id);
            }
            AMSketch c = null;
            if(incr)
                c = count.attackIncr(id);
            else
                c = count.attackDecr(id);
            result = new Phase1Message(p, X, ((locY == null) ? null : Y), c, fixedPoint, timeBlock);
            result.sigVerify();
        }else{
            try {
                result = new FakePhase1Message(0, this.fixedPoint, new Location.Location2D(0,0), -1, this.timeBlock, this.estimate() + 0.5 * SeDyAApp.AGREEMENT_THRESHOLD, this.getEstimateLocation(), this.participantCount() * 2);
            } catch (PropertyNotFoundException ex) {
                throw new RuntimeException("Error while generating FakePhase1Message; property "+ ex.getMessage()  +" not found!");
            }

        }
        if(result==null || result.getSize() > NetIpBase.MAXIMUM_PAYLOAD_SIZE)
            return null;
        result.attacked();
        return result;
    }
    private boolean isAttacked = false;

    private void attacked() {
        isAttacked = true;
    }

    public boolean isAttacked() {
        return this.isAttacked;
    }

    /**
     * Determine the estimated amount of participants according to the message payload.
     * @return
     */
    public double participantCount() {
        return this.count.estimate();
    }

    public void printit() {
        ((AMFM_PCSASketch)this.payload).print();

        //System.err.println(this.count);
        if(locX!=null)
        ((AMFM_PCSASketch)this.locX).print();
    }
}
