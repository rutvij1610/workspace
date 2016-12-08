package sedya.data;

import java.util.Collection;

public class Signature extends AbstractSignature{
    /**Size of a signature in bytes. TODO verify this number.*/
    public static final int SIZE = 65;
    private final Identity generator;
    public Signature(Identity id){
        generator=id;
    }
    @Override
    public boolean checkSignature(Identity id){
        return generator.get()==id.get();
    }
    @Override
    public boolean checkSignature(Collection<Identity> id){
        return false;
    }

}
