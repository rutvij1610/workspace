package sedya.data;

import ducks.driver.SeDyANodes;
import jist.swans.misc.Location;
import sedya.apps.SeDyAApp;

public class FinalizedMessage extends Phase2Message {

    /** signature of finalizing vehicle. */
    private MultiSignature s;
    /** identity/pseudonym of finalizing vehicle. */
    private Identity id;

    public FinalizedMessage(Phase1Message content, Location l) {
        super(content, l);
        SeDyANodes.performAreaEvaluation(this);
    }

    protected FinalizedMessage(Phase1Message content, Location l,
            Identity id, MultiSignature s) {
        super(content, l);
        this.id = id;
        this.s = s;
    }

    /**
     * Obtain the size of this message.
     * @return
     */
    public int getSize() {
        //         content       signature        identity        location
        if(SeDyAApp.securityEnabled)
            return m.getSize() + Signature.SIZE + Identity.SIZE + (Float.SIZE * 2 / 8);
        else
            return m.getSize() + (Float.SIZE * 2 / 8);
    }

    /**
     * Not implemented. Will return without modification of message.
     */
    public void getBytes(byte[] msg, int offset) {
        return;
    }

    /** Generate a signature on this message. */
    public void createSignature(Identity id) {
        this.s = new MultiSignature(id);
        this.id = id;
    }

    /**
     * verify the "outer" signature.
     * @return
     */
    public boolean verifySignature() {
        return this.s.checkSignature(this.id);
    }

    /**
     * verify phase 1 security material.
     * @return
     */
    public boolean verifyPhase1() {
        return this.m.sigVerify();
    }

    public boolean isStripped() {
        return m.isStripped();
    }

    /**
     * Get the long representation of the identity used to sign this message.
     * @return 
     */
    public long fetchId() {
        if(id==null)
            System.err.println("Error, message not signed!");
        return id.get();
    }

    /*=================================================
     *  Queries to the contained Phase1Message.
     *
     */
    /**
     * Perform checks against a physical model.
     * @return true if checks are passed; false otherwise.
     */
    public boolean phyModelChecks() {
        return m.phyModelChecks();
    }

    public FinalizedMessage stripPhase1Security() {
        Phase1Message tmp = m.stripSecurityPayload();
        return new FinalizedMessage(tmp, this.loc, this.id, this.s);
    }

    public boolean containsLocation(Location l) {
        return containsBufferedLocation(l, SeDyAApp.BUFF);
    }
        
    public boolean containsBufferedLocation(Location l, double buffer){
        Location[] spec = this.getLocation();
        if (this.m.isHighway()) {
            Location opposite = spec[1].mirrorIn(spec[0]);
            Location max = new Location.Location2D(
                    (float) (StrictMath.max(spec[1].getX(), opposite.getX()) + buffer),
                    (float) (StrictMath.max(spec[1].getY(), opposite.getY()) + buffer));
            Location min = new Location.Location2D(
                    (float) (StrictMath.min(spec[1].getX(), opposite.getX()) - buffer),
                    (float) (StrictMath.min(spec[1].getY(), opposite.getY()) - buffer));
            
            return l.inside(min, max);
        } else {

            return StrictMath.abs(spec[0].distance(l))
                    < (StrictMath.abs(spec[0].distance(spec[1])) + SeDyAApp.BUFF);
        }
    }

    public void printStatistics() {
        //System.err.println("Message " + m.id + ": "+ "(t=" +this.getTimeBlock()+")"+m.getObservations() + " | " + m.getParticipants() );
        return;
    }
}
