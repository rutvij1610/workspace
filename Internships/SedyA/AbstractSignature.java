package sedya.data;

import java.util.Collection;


public abstract class AbstractSignature {

    public boolean checkSignature(Object id) {
        if (id instanceof Identity) {
            return this.checkSignature((Identity) id);
        } else if (id instanceof Collection) {
            return this.checkSignature((Collection<Identity>) id);
        }
        return false;
    }

    /**
     * Verify using a single "public key"
     * @param id the identity associated with the public key (ie. the public key
     * , since we represent IBE)
     * @return true iff the signature is valid.
     */
    abstract boolean checkSignature(Identity id);

    /**
     * Verify using a collection of "public keys"
     * @param id the collection of identities that cooperated to create this
     * signature (ie. the public keys, since we represent IBE)
     * @return true iff the signature is valid.
     */
    abstract boolean checkSignature(Collection<Identity> id);
}
