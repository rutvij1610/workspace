package sedya.attackers;

import ducks.driver.SeDyANode;
import ext.util.PropertyNotFoundException;
import jist.swans.field.Field.RadioData;
import jist.swans.misc.Location;
import jist.swans.misc.Location.Location2D;
import sedya.apps.Phase1Abstract;
import sedya.apps.SeDyAApp;
import sedya.data.Phase1Message;
import vans.net.NeighborTable;


public class Phase1NaiveAttacker extends Phase1Abstract {

    /** Maximum possible speed in m/s */
    public static final int MAX = 100;

    public Phase1NaiveAttacker(SeDyANode n, int nodeId, NeighborTable nt, RadioData rd, Location2D[] l) {
        super(n, nodeId, nt, rd, l);
    }

    @Override
    protected Phase1Message observe() {
        Location l = rd.getLoc();

        //select fixed point:
        float fX = (l.getX() % (this.getTopRight().getX() / (SeDyAApp.FIXED_POSITION_INTERVAL_X)));
        float fY = (l.getY() % (this.getTopRight().getY() / (SeDyAApp.FIXED_POSITION_INTERVAL_Y)));

        //payload will be speed: this attacker randomizes.
        int data = jist.swans.Constants.random.nextInt(MAX);
        int x = (int) (l.getX() - fX);
        int y = (int) (l.getY() - fY);
        //generate timestamp
        int time = SeDyAApp.getCurrentTimeBlock();

        //create Phase1Message:
        try {
            data1 = data;
        this.observationLocation = l;
        this.observationSpeed = data;
            return new Phase1Message(data, new Location2D(fX, fY), new Location2D(x, y), this.id, time);
        } catch (PropertyNotFoundException e) {
            throw new RuntimeException("Error parsing parameters!\n" + e.getMessage());
        }
    }
}
