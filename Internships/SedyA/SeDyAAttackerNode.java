package sedya.attackers;

import ducks.misc.GlobalConfig;
import ducks.driver.SeDyANode;
import ducks.driver.SeDyASimParams;
import sedya.apps.Phase2;
import sedya.apps.SeDyAApp;
import vans.net.NeighborTable;

public class SeDyAAttackerNode extends SeDyANode {

    @Override
    protected void createPhases(NeighborTable neighborTable) {
        String t = GlobalConfig.instance().getConfig().getStringProperty(SeDyASimParams.SEDYA_ATTACKER_TYPE, null);
        if (t==null || !t.startsWith(SeDyASimParams.SEDYA_ATTACKER_PHASE1)) {
            super.createPhase1(neighborTable);
        } else {
            if (t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE1_N)) {
                //naive
                this.p1 = new Phase1NaiveAttacker(this, this.id, neighborTable, scene.getField().getRadioData(new Integer(this.id)), scene.getCorners());
            } else if (t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE1_R)) {
                //resize
                this.p1 = new Phase1ResizeAttacker(this, this.id, neighborTable, scene.getField().getRadioData(new Integer(this.id)), scene.getCorners(), false);
            } else if (t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE1_S)) {
                //smart
                this.p1 = new Phase1SmartAttacker(this, this.id, neighborTable, scene.getField().getRadioData(new Integer(this.id)), scene.getCorners());
            } else {
                throw new RuntimeException("Unknown attacker type " + t);
            }
            this.p1.setNetEntity(this.netEntity);
            this.protocolMap.testMapToNext(vans.Constants.NET_PROTOCOL_SEDYA + SeDyAApp.PHASE1);
            this.net.setProtocolHandler(vans.Constants.NET_PROTOCOL_SEDYA + SeDyAApp.PHASE1, this.p1.getNetHandlerProxy());
        }
        if (t==null || !t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE2)) {
            super.createPhase2(neighborTable);
        } else {
            if (t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE2_A)) {
                //area
                this.p2 = new Phase2AreaAttacker(this, this.id, neighborTable, scene.getField().getRadioData(new Integer(this.id)));
            } else if (t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE2_D)) {
                //decrement
                this.p2 = new Phase2DecrementAttacker(this, this.id, neighborTable, scene.getField().getRadioData(new Integer(this.id)));
            } else if ((t.equals(SeDyASimParams.SEDYA_ATTACKER_PHASE2_I))) {
                //increment
                this.p2 = new Phase2IncrementAttacker(this, this.id, neighborTable, scene.getField().getRadioData(new Integer(this.id)));
            } else {
                throw new RuntimeException("Unknown attacker type " + t);
            }
            this.p2.setNetEntity(this.netEntity);
            this.protocolMap.testMapToNext(vans.Constants.NET_PROTOCOL_SEDYA + SeDyAApp.PHASE2);
            this.net.setProtocolHandler(vans.Constants.NET_PROTOCOL_SEDYA + SeDyAApp.PHASE2, this.p2.getNetHandlerProxy());
            this.p1.registerDisagreementListener(((Phase2) this.p2));

        }

        super.createPhase3(neighborTable);

    }
}
